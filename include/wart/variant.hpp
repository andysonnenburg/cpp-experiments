#ifndef WART_VARIANT_HPP
#define WART_VARIANT_HPP

#include <type_traits>

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
	}

	template <typename... Types>
	class variant {
		int which_;
		union_t<Types...> union_;

	public:
		template <typename T>
		variant(T const& value): which_(detail::elem_index<T, Types...>::value) {
			new (&union_) T(value);
		}

		template <typename F>
		typename std::common_type<std::result_of<F(Types)>::type...>::type accept(F&& f) const {
			using result_type std::common_type<std::result_of<F(Types)>::type...>::type
			using call = result_type (*)(F&& f, union_t<Types...> const&);
			static call calls[] {
				[](F&& f, union_t<Types...> const& value) {
					return f(*static_cast<Types const*>(static_cast<void const*>(&value)));
				}...
			};
			return calls[which_](std::forward<F>(f), union_);
		}

		template <typename F>
		typename std::common_type<std::result_of<F(Types)>::type...>::type accept(F&& f) {
			using result_type std::common_type<std::result_of<F(Types)>::type...>::type
			using call = result_type (*)(F&& f, union_t<Types...>&);
			static call calls[] {
				[](F&& f, union_t<Types...>& value) {
					return f(*static_cast<Types*>(static_cast<void*>(&value)));
				}...
			};
			return calls[which_](std::forward<F>(f), union_);
		}
	};
}

#endif
