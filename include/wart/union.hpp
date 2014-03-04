#ifndef WART_UNION_HPP
#define WART_UNION_HPP

#include "union/detail/union.hpp"

#include "all.hpp"

#include <type_traits>

namespace wart {

template <typename... T>
using union_t =
	detail::union_t::union_t<
	all<std::is_trivially_destructible<T>::value...>::value,
	T...>;

template <typename Elem, typename... T>
constexpr Elem& union_get(union_t<T...>& value) {
	return detail::union_t::get<Elem, T...>::call(value);
}

template <typename Elem, typename... T>
constexpr Elem const& union_get(union_t<T...> const& value) {
	return detail::union_t::get<Elem, T...>::call(value);
}

template <typename Elem, typename... T>
constexpr Elem&& union_get(union_t<T...>&& value) {
	return detail::union_t::get<Elem, T...>::call(std::move(value));
}

}

#endif
