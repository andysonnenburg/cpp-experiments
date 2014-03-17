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
	using type = T;
};

template <typename T, typename U>
struct undecayed_common_type<T, U> {
	using type = decltype(true? std::declval<T>() : std::declval<U>());
};

template <typename T, typename U, typename... V>
struct undecayed_common_type<T, U, V...> {
	using type = undecayed_common_type_t<undecayed_common_type_t<T, U>, V...>;
};

}

#endif
