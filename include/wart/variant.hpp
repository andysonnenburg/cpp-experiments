#ifndef WART_VARIANT_HPP
#define WART_VARIANT_HPP

#include <type_traits>

#include "union.hpp"

namespace wart {
	namespace detail {
		template <int Index, typename Elem, typename Head, typename... Tail>
		struct elem_index;

		template <int Index, typename Head, typename... Tail>
		struct elem_index<Index, Head, Head, Tail...>:
			std::integral_constant<int, Index> {};

		template <int Index, typename Elem, typename Head, typename... Tail>
		struct elem_index: elem_index<Index + 1, Elem, Tail...> {};
	}

	template <typename... Types>
	class variant {
		int which_;
		union_t<Types...> union_;

	public:
		template <typename T>
		variant(T const& value): which_(detail::elem_index<0, T, Types...>::value) {
			new (&union_) T(value);
		}

		template <typename F>
		typename F::result_type accept(F&& f) const {
			using call = typename F::result_type (*)(F&& f, union_t<Types...> const&);
			static call calls[] {
				[](F&& f, union_t<Types...> const& value) {
					return f(*static_cast<Types const*>(static_cast<void const*>(&value)));
				}...
			};
			return calls[which_](std::forward<F>(f), union_);
		}
	};
}

#endif
