#ifndef WART_VARIANT_HPP
#define WART_VARIANT_HPP

#include <type_traits>
#include <utility>

#include "union.hpp"

namespace wart {
	namespace detail {
		template <typename Elem, typename Head, typename... Tail>
		struct elem_index;

		template <typename Head, typename... Tail>
		struct elem_index<Head, Head, Tail...>:
			std::integral_constant<int, 0> {};

		template <typename Elem, typename Head, typename... Tail>
		struct elem_index:
			std::integral_constant<int, elem_index<Elem, Tail...>::value + 1> {};

		template <typename Elem, typename... List>
		struct elem;

		template <typename Head, typename... Tail>
		struct elem<Head, Head, Tail...>: std::true_type {};

		template <typename Elem, typename Head, typename... Tail>
		struct elem<Elem, Head, Tail...>: elem<Elem, Tail...>::type {};

		template <typename Elem>
		struct elem<Elem>: std::false_type {};

		template <bool... List>
		struct all;

		template <>
		struct all<>: std::true_type {};

		template <bool... Tail>
		struct all<true, Tail...>: all<Tail...>::type {};

		template <bool... Tail>
		struct all<false, Tail...>: std::false_type {};

		struct construct {
			void* memory;
			template <typename T>
			void operator()(T const& value) {
				new (memory) T(value);
			}
		};

		struct assign {
			void* memory;
			template <typename T>
			void operator()(T const& value) {
				*static_cast<T*>(memory) = value;
			}
		};

		struct destroy {
			template <typename T>
			void operator()(T const& value) {
				value.~T();
			}
		};

		template <typename T, typename... Types>
		inline
		typename std::enable_if<elem<T, Types...>::value, T&>::type
		union_cast(union_t<Types...>& value) {
			return *static_cast<T*>(static_cast<void*>(&value));
		}

		template <typename T, typename... Types>
		inline
		typename std::enable_if<elem<T, Types...>::value, T const&>::type
		union_cast(union_t<Types...> const& value) {
			return *static_cast<T const*>(static_cast<void const*>(&value));
		}
	}

	template <typename... Types>
	class variant {
		int which_;
		union_t<Types...> union_;

	public:
		template <typename T>
		variant(T const& value): which_{detail::elem_index<T, Types...>::value} {
			new (&union_) T{value};
		}

		variant(variant const& rhs): which_{rhs.which_} {
			rhs.accept(detail::construct{&union_});
		}

		~variant() {
			accept(detail::destroy{});
		}

		typename std::enable_if<
			detail::all<std::is_nothrow_constructible<Types...>::value>::value,
			variant const&
			>::type operator=(variant const& rhs) {
			if (which_ == rhs.which_) {
				rhs.accept(detail::assign{&union_});
			} else {
				accept(detail::destroy{});
				which_ = rhs.which_;
				rhs.accept(detail::construct{&union_});
			}
			return *this;
		}

		template <typename F>
		typename std::common_type<
			typename std::result_of<F(Types)>::type...
			>::type accept(F&& f) const {
			using result_type = typename std::common_type<
				typename std::result_of<F(Types)>::type...
				>::type;
			using call = result_type (*)(F&& f, union_t<Types...> const&);
			static call calls[] {
				[](F&& f, union_t<Types...> const& value) {
					return std::forward<F>(f)(detail::union_cast<Types, Types...>(value));
				}...
			};
			return calls[which_](std::forward<F>(f), union_);
		}

		template <typename F>
		typename std::common_type<
			typename std::result_of<F(Types)>::type...
			>::type accept(F&& f) {
			using result_type = typename std::common_type<
				typename std::result_of<F(Types)>::type...
				>::type;
			using call = result_type (*)(F&& f, union_t<Types...>&);
			static call calls[] {
				[](F&& f, union_t<Types...>& value) {
					return std::forward<F>(f)(detail::union_cast<Types, Types...>(value));
				}...
			};
			return calls[which_](std::forward<F>(f), union_);
		}
	};
}

#endif
