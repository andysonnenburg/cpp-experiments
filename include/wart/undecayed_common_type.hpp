#ifndef WART_UNDECAYED_COMMON_TYPE_HPP
#define WART_UNDECAYED_COMMON_TYPE_HPP

#include <type_traits>
#include <utility>

namespace wart {

template <typename... T>
struct undecayed_common_type;

template <typename... T>
using undecayed_common_type_t = typename undecayed_common_type<T...>::type;

template <typename T>
struct undecayed_common_type<T> {
	typedef T type;
};

template <typename T, typename U>
struct undecayed_common_type<T, U> {
	typedef decltype(true? std::declval<T>() : std::declval<U>()) type;
};

template <typename Head, typename... Tail>
struct undecayed_common_type<Head, Tail...> {
	typedef undecayed_common_type_t<Head, undecayed_common_type_t<Tail...>> type;
};

}

#endif
