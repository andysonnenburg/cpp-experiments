#ifndef WART_VARIANT_DETAIL_ELEM_HPP
#define WART_VARIANT_DETAIL_ELEM_HPP

#include <type_traits>

namespace wart { namespace detail { namespace variant {

template <typename Elem, typename... List>
struct elem;

template <typename Head, typename... Tail>
struct elem<Head, Head, Tail...>: std::true_type {};

template <typename Elem, typename Head, typename... Tail>
struct elem<Elem, Head, Tail...>: elem<Elem, Tail...>::type {};

template <typename Elem>
struct elem<Elem>: std::false_type {};

}}}

#endif
