#ifndef WART_VARIANT_DETAIL_ELEM_INDEX_HPP
#define WART_VARIANT_DETAIL_ELEM_INDEX_HPP

#include <type_traits>

namespace wart { namespace detail { namespace variant {

template <typename Elem, typename... List>
struct elem_index;

template <typename Head, typename... Tail>
struct elem_index<Head, Head, Tail...>:
		std::integral_constant<int, 0> {};

template <typename Elem, typename Head, typename... Tail>
struct elem_index<Elem, Head, Tail...>:
		std::integral_constant<int, elem_index<Elem, Tail...>::value + 1> {};

}}}

#endif
