#ifndef WART_VARIANT_HPP
#define WART_VARIANT_HPP

#include <type_traits>
#include <utility>

#include "union_storage.hpp"

namespace wart {
	namespace detail {
		namespace variant {
			template <typename Elem, typename Head, typename... Tail>
			struct elem_index;

			template <typename Head, typename... Tail>
			struct elem_index<Head, Head, Tail...>:
				std::integral_constant<int, 0> {};

			template <typename Elem, typename Head, typename... Tail>
			struct elem_index:
				std::integral_constant<int, elem_index<Elem, Tail...>::value + 1> {};

			template <bool... List>
			struct all;

			template <>
			struct all<>: std::true_type {};

			template <bool... Tail>
			struct all<true, Tail...>: all<Tail...>::type {};

			template <bool... Tail>
			struct all<false, Tail...>: std::false_type {};

			struct copier {
				void* storage;
				template <typename T>
				void operator()(T const& value) {
					new (storage) T(value);
				}
			};

			struct move_copier {
				void* storage;
				template <typename T>
				void operator()(T&& value) {
					new (storage) T(std::move(value));
				}
			};

			struct assigner {
				void* storage;
				template <typename T>
				void operator()(T const& value) {
					*static_cast<T*>(storage) = value;
				}
			};

			struct destroyer {
				template <typename T>
				void operator()(T const& value) {
					value.~T();
				}
			};
		}
	}

	template <typename... Types>
	class variant {
		int which_;
		union_storage<Types...> union_storage_;

	public:
		template <typename T>
		variant(T const& value): which_{detail::variant::elem_index<T, Types...>::value} {
			new (&union_storage_) T(value);
		}

		variant(variant const& rhs): which_{rhs.which_} {
			rhs.accept(detail::variant::copier{&union_storage_});
		}

		variant(variant&& rhs): which_{rhs.which_} {
			std::move(rhs).accept(detail::variant::move_copier{&union_storage_});
		}

		~variant() {
			accept(detail::variant::destroyer{});
		}

		variant& operator=(variant const& rhs) & {
			using detail::variant::all;
			static_assert(all<std::is_nothrow_constructible<Types>::value...>::value,
			              "all of template arguments Types must be nothrow constructible in class template variant");
			if (this == &rhs) {
				return *this;
			}
			if (which_ == rhs.which_) {
				rhs.accept(detail::variant::assigner{&union_storage_});
			} else {
				accept(detail::variant::destroyer{});
				which_ = rhs.which_;
				rhs.accept(detail::variant::copier{&union_storage_});
			}
			return *this;
		}

		template <typename F>
		typename std::common_type<
			typename std::result_of<F(Types)>::type...
			>::type accept(F&& f) const& {
			using result_type = typename std::common_type<
				typename std::result_of<F(Types)>::type...
				>::type;
			using call = result_type (*)(F&& f, union_storage<Types...> const&);
			static call calls[] {
				[](F&& f, union_storage<Types...> const& value) {
					return std::forward<F>(f)(union_cast<Types, Types...>(value));
				}...
			};
			return calls[which_](std::forward<F>(f), union_storage_);
		}

		template <typename F>
		typename std::common_type<
			typename std::result_of<F(Types)>::type...
			>::type accept(F&& f) & {
			using result_type = typename std::common_type<
				typename std::result_of<F(Types)>::type...
				>::type;
			using call = result_type (*)(F&& f, union_storage<Types...>&);
			static call calls[] {
				[](F&& f, union_storage<Types...>& value) {
					return std::forward<F>(f)(union_cast<Types, Types...>(value));
				}...
			};
			return calls[which_](std::forward<F>(f), union_storage_);
		}

		template <typename F>
		typename std::common_type<
			typename std::result_of<F(Types)>::type...
			>::type accept(F&& f) && {
			using result_type = typename std::common_type<
				typename std::result_of<F(Types)>::type...
				>::type;
			using call = result_type (*)(F&& f, union_storage<Types...>&&);
			static call calls[] {
				[](F&& f, union_storage<Types...>&& value) {
					return std::forward<F>(f)(std::move(union_cast<Types, Types...>(value)));
				}...
			};
			return calls[which_](std::forward<F>(f), std::move(union_storage_));
		}
	};
}

#endif
