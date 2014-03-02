#ifndef WART_VARIANT_HPP
#define WART_VARIANT_HPP

#include <type_traits>
#include <utility>

#include "math.hpp"

namespace wart {
	template <typename... T>
	class variant;

	namespace detail {
		namespace variant {
			template <typename... T>
			using variant = wart::variant<T...>;

			template <typename T>
			using is_movable = typename std::integral_constant
				<bool,
				 std::is_rvalue_reference<T&&>::value && !std::is_const<T>::value>;

			template <typename T, typename U = void>
			using enable_if_movable = std::enable_if<is_movable<T>::value, U>;

			template <typename... Types>
			using union_storage = typename std::aligned_storage
				<math::max_constant<std::size_t,
				                    sizeof(Types)...>::value,
				 math::lcm_constant<std::size_t,
				                    std::alignment_of<Types>::value...>::value>::type;

			template <typename... Types>
			using union_storage_t = typename union_storage<Types...>::type;

			template <typename Elem, typename... List>
			struct elem;

			template <typename Head, typename... Tail>
			struct elem<Head, Head, Tail...>: std::true_type {};

			template <typename Elem, typename Head, typename... Tail>
			struct elem<Elem, Head, Tail...>: elem<Elem, Tail...>::type {};

			template <typename Elem>
			struct elem<Elem>: std::false_type {};

			template <typename Elem, typename... List>
			struct elem_index;

			template <typename Head, typename... Tail>
			struct elem_index<Head, Head, Tail...>:
				std::integral_constant<int, 0> {};

			template <typename Elem, typename Head, typename... Tail>
			struct elem_index<Elem, Head, Tail...>:
				std::integral_constant<int, elem_index<Elem, Tail...>::value + 1> {};

			template <bool... List>
			struct all;

			template <>
			struct all<>: std::true_type {};

			template <bool... Tail>
			struct all<true, Tail...>: all<Tail...>::type {};

			template <bool... Tail>
			struct all<false, Tail...>: std::false_type {};

			template <typename Elem, typename... List>
			using enable_if_elem = std::enable_if<elem<Elem, List...>::value>;

			template <typename F, typename... ArgTypes>
			using common_result_of =
				std::common_type<typename std::result_of<F(ArgTypes)>::type...>;

			struct destroy {
				template <typename T>
				void operator()(T&& value) {
					using type = typename std::remove_reference<T>::type;
					std::forward<T>(value).~type();
				}
			};

			struct copy_construct {
				void* storage;
				template <typename T>
				void operator()(T const& value) {
					new (storage) T(value);
				}
			};

			template <typename... T>
			struct copy_construct_index {
				void* storage;
				template <typename U>
				int operator()(U const& value) {
					new (storage) U(value);
					return elem_index<U, T...>::value;
				}
			};

			struct move_construct {
				void* storage;
				template <typename T>
				typename enable_if_movable<T>::type operator()(T&& value) {
					new (storage) T(std::move(value));
				}
			};

			template <typename... T>
			struct move_construct_index {
				void* storage;
				template <typename U>
				typename enable_if_movable<U, int>::type operator()(U&& value) {
					new (storage) U(std::move(value));
					return elem_index<U, T...>::value;
				}
			};

			struct copy_assign {
				void* storage;
				template <typename T>
				void operator()(T const& value) {
					*static_cast<T*>(storage) = value;
				}
			};

			template <typename... T>
			struct copy_assign_reindex {
				variant<T...>& variant;
				template <typename U>
				void operator()(U const& value) {
					if (variant.which_ == elem_index<U, T...>::value) {
						*static_cast<U*>(static_cast<void*>(&variant.storage_)) = value;
					} else {
						variant.accept(destroy{});
						new (&variant.storage_) U(value);
						variant.which_ = elem_index<U, T...>::value;
					}
				}
			};

			struct move_assign {
				void* storage;
				template <typename T>
				typename enable_if_movable<T>::type operator()(T&& value) {
					*static_cast<T*>(storage) = std::move(value);
				}
			};

			template <typename... T>
			struct move_assign_reindex {
				variant<T...>& variant;
				template <typename U>
				typename enable_if_movable<U>::type operator()(U&& value) {
					if (variant.which_ == elem_index<U, T...>::value) {
						*static_cast<U*>(static_cast<void*>(&variant.storage_)) = std::move(value);
					} else {
						variant.accept(destroy{});
						new (&variant.storage_) U(std::move(value));
						variant.which_ = elem_index<U, T...>::value;
					}
				}
			};
		}
	}

	template <typename... T>
	class variant {
		int which_;
		detail::variant::union_storage_t<T...> storage_;

	public:
		template <typename F>
		using result_of = detail::variant::common_result_of<F, T...>;
		template <typename F>
		using result_of_t = typename result_of<F>::type;

		template <typename U>
		variant(U const& value,
		        typename detail::variant::enable_if_elem<U, T...>::type* = nullptr):
			which_{detail::variant::elem_index<U, T...>::value} {
			new (&storage_) U(value);
		}

		template <typename U>
		variant(U&& value,
		        typename detail::variant::enable_if_elem<U, T...>::type* = nullptr,
		        typename detail::variant::enable_if_movable<U>::type* = nullptr):
			which_{detail::variant::elem_index<U, T...>::value} {
			new (&storage_) U(std::move(value));
		}

		variant(variant const& rhs):
			which_{rhs.which_} {
			rhs.accept(detail::variant::copy_construct{&storage_});
		}

		template <typename... U>
		variant(variant<U...> const& rhs,
		        typename std::enable_if<
		        detail::variant::all<detail::variant::elem<U, T...>::value...>::value
		        >::type* = nullptr):
			which_{rhs.accept(detail::variant::copy_construct_index<T...>{&storage_})} {}

		variant(variant&& rhs):
			which_{rhs.which_} {
			std::move(rhs).accept(detail::variant::move_construct{&storage_});
		}

		template <typename... U>
		variant(variant<U...>&& rhs,
		        typename std::enable_if<
		        detail::variant::all<detail::variant::elem<U, T...>::value...>::value
		        >::type* = nullptr):
			which_{std::move(rhs).accept(detail::variant::move_construct_index<T...>{&storage_})} {}

		~variant() {
			accept(detail::variant::destroy{});
		}

		variant& operator=(variant const& rhs) & {
			using namespace detail::variant;
			static_assert(all<std::is_nothrow_copy_constructible<T>::value...>::value,
			              "all template arguments T must be nothrow copy constructible in class template variant");
			if (this == &rhs) {
				return *this;
			}
			if (which_ == rhs.which_) {
				rhs.accept(copy_assign{&storage_});
			} else {
				accept(destroy{});
				rhs.accept(copy_construct{&storage_});
				which_ = rhs.which_;
			}
			return *this;
		}

		template <typename... U>
		variant& operator=(variant<U...> const& rhs) & {
			using namespace detail::variant;
			static_assert(all<std::is_nothrow_copy_constructible<T>::value...>::value,
			              "all template arguments T must be nothrow copy constructible in class template variant");
			rhs.accept(copy_assign_reindex<T...>{*this});
			return *this;
		}

		variant& operator=(variant&& rhs) & {
			using namespace detail::variant;
			static_assert(all<std::is_nothrow_move_constructible<T>::value...>::value,
			              "all template arguments T must be nothrow move constructible in class template variant");
			if (this == &rhs) {
				return *this;
			}
			if (which_ == rhs.which_) {
				std::move(rhs).accept(move_assign{&storage_});
			} else {
				accept(detail::variant::destroy{});
				std::move(rhs).accept(move_construct{&storage_});
				which_ = rhs.which_;
			}
			return *this;
		}

		template <typename... U>
		variant& operator=(variant<U...>&& rhs) & {
			using namespace detail::variant;
			static_assert(all<std::is_nothrow_copy_constructible<T>::value...>::value,
			              "all template arguments T must be nothrow copy constructible in class template variant");
			std::move(rhs).accept(move_assign_reindex<T...>{*this});
			return *this;
		}

		template <typename F>
		result_of_t<F> accept(F&& f) const& {
			using namespace detail::variant;
			using call = result_of_t<F&&> (*)(F&& f, union_storage_t<T...> const&);
			static call calls[] {
				[](F&& f, union_storage_t<T...> const& value) {
					return std::forward<F>(f)(*static_cast<T const*>(static_cast<void const*>(&value)));
				}...
			};
			return calls[which_](std::forward<F>(f), storage_);
		}

		template <typename F>
		result_of_t<F> accept(F&& f) & {
			using namespace detail::variant;
			using call = result_of_t<F&&> (*)(F&& f, union_storage_t<T...>&);
			static call calls[] {
				[](F&& f, union_storage_t<T...>& value) {
					return std::forward<F>(f)(*static_cast<T*>(static_cast<void*>(&value)));
				}...
			};
			return calls[which_](std::forward<F>(f), storage_);
		}

		template <typename F>
		result_of_t<F> accept(F&& f) && {
			using namespace detail::variant;
			using call = result_of_t<F> (*)(F&& f, union_storage_t<T...>&&);
			static call calls[] {
				[](F&& f, union_storage_t<T...>&& value) {
					return std::forward<F>(f)(std::move(*static_cast<T*>(static_cast<void*>(&value))));
				}...
			};
			return calls[which_](std::forward<F>(f), std::move(storage_));
		}

		friend
		struct detail::variant::copy_assign_reindex<T...>;

		friend
		struct detail::variant::move_assign_reindex<T...>;
	};
}

#endif
