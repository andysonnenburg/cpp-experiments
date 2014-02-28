#ifndef WART_VARIANT_HPP
#define WART_VARIANT_HPP

#include <iostream>
#include <type_traits>
#include <utility>

#include "union_storage.hpp"

namespace wart {
	namespace detail {
		namespace variant {
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

			template <typename T>
			using is_movable = typename std::integral_constant
				<bool,
				 std::is_rvalue_reference<T&&>::value && !std::is_const<T>::value>;

			template <typename T, typename U = void>
			using enable_if_movable = std::enable_if<is_movable<T>::value, U>;

			template <typename F, typename... ArgTypes>
			using common_result_of =
				std::common_type<typename std::result_of<F(ArgTypes)>::type...>;

			struct copy_construct {
				void* storage;
				template <typename T>
				void operator()(T const& value) {
					new (storage) T(value);
				}
			};

			struct move_construct {
				void* storage;
				template <typename T>
				typename enable_if_movable<T>::type operator()(T&& value) {
					new (storage) T(std::move(value));
				}
			};

			struct copy_assign {
				void* storage;
				template <typename T>
				void operator()(T const& value) {
					*static_cast<T*>(storage) = value;
				}
			};

			struct move_assign {
				void* storage;
				template <typename T>
				typename enable_if_movable<T>::type operator()(T&& value) {
					*static_cast<T*>(storage) = std::move(value);
				}
			};

			struct destroy {
				template <typename T>
				void operator()(T&& value) {
					using type = typename std::remove_reference<T>::type;
					std::forward<T>(value).~type();
				}
			};
		}
	}

	template <typename... Types>
	class variant {
		int which_;
		union_storage<Types...> union_storage_;

	public:
		template <typename F>
		using result_of = detail::variant::common_result_of<F, Types...>;
		template <typename F>
		using result_of_t = typename result_of<F>::type;

		template <typename T>
		variant(T const& value):
			which_{detail::variant::elem_index<T, Types...>::value} {
			new (&union_storage_) T(value);
		}

		template <typename T>
		variant(T&& value):
			which_{detail::variant::elem_index<T, Types...>::value} {
			new (&union_storage_) T(std::move(value));
		}

		variant(variant& rhs):
			variant(static_cast<const variant&>(rhs)) {}

		variant(variant const& rhs):
			which_{rhs.which_} {
			rhs.accept(detail::variant::copy_construct{&union_storage_});
		}

		variant(variant&& rhs):
			which_{rhs.which_} {
			std::move(rhs).accept(detail::variant::move_construct{&union_storage_});
		}

		~variant() {
			accept(detail::variant::destroy{});
		}

		variant& operator=(variant const& rhs) & {
			using namespace detail::variant;
			static_assert(all<std::is_nothrow_copy_constructible<Types>::value...>::value,
			              "all template arguments Types must be nothrow copy constructible in class template variant");
			if (this == &rhs) {
				return *this;
			}
			if (which_ == rhs.which_) {
				rhs.accept(copy_assign{&union_storage_});
			} else {
				accept(destroy{});
				which_ = rhs.which_;
				rhs.accept(copy_construct{&union_storage_});
			}
			return *this;
		}

		variant& operator=(variant&& rhs) & {
			using namespace detail::variant;
			static_assert(all<std::is_nothrow_move_constructible<Types>::value...>::value,
			              "all template arguments Types must be nothrow move constructible in class template variant");
			if (this == &rhs) {
				return *this;
			}
			if (which_ == rhs.which_) {
				std::move(rhs).accept(move_assign{&union_storage_});
			} else {
				accept(detail::variant::destroy{});
				which_ = rhs.which_;
				std::move(rhs).accept(move_construct{&union_storage_});
			}
			return *this;
		}

		template <typename F>
		result_of_t<F> accept(F&& f) const& {
			using call = result_of_t<F> (*)(F&& f, union_storage<Types...> const&);
			static call calls[] {
				[](F&& f, union_storage<Types...> const& value) {
					return std::forward<F>(f)(union_cast<Types, Types...>(value));
				}...
			};
			return calls[which_](std::forward<F>(f), union_storage_);
		}

		template <typename F>
		result_of_t<F> accept(F&& f) & {
			using call = result_of_t<F> (*)(F&& f, union_storage<Types...>&);
			static call calls[] {
				[](F&& f, union_storage<Types...>& value) {
					return std::forward<F>(f)(union_cast<Types, Types...>(value));
				}...
			};
			return calls[which_](std::forward<F>(f), union_storage_);
		}

		template <typename F>
		result_of_t<F> accept(F&& f) && {
			using call = result_of_t<F> (*)(F&& f, union_storage<Types...>&&);
			static call calls[] {
				[](F&& f, union_storage<Types...>&& value) {
					return std::forward<F>(f)(union_cast<Types, Types...>(std::move(value)));
				}...
			};
			return calls[which_](std::forward<F>(f), std::move(union_storage_));
		}
	};
}

#endif