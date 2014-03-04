#ifndef WART_VARIANT_DETAIL_ALL_HPP
#define WART_VARIANT_DETAIL_ALL_HPP

#include <type_traits>

namespace wart { namespace detail { namespace variant {

template <bool...>
struct all;

template <>
struct all<>: std::true_type {};

template <bool... Tail>
struct all<false, Tail...>: std::false_type {};

template <bool... Tail>
struct all<true, Tail...>: all<Tail...>::type {};

}}}

#endif
