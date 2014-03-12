#ifndef WART_UNION_HPP
#define WART_UNION_HPP

#include "union/detail.hpp"

#include "all.hpp"
#include "enable_if_move_constructible.hpp"

#include <type_traits>
#include <utility>

namespace wart {

template <typename... T>
class union_t {
	detail::union_t::union_t<
		all<std::is_trivially_destructible<T>::value...>::value,
		T...> union_;

public:
	constexpr union_t()
		: union_() {}

	template <typename U>
	constexpr union_t(U const& value):
		union_(value) {}

	template <typename U>
	constexpr union_t(U&& value,
	                  typename enable_if_move_constructible<U>::type* = nullptr):
		union_(std::move(value)) {}

	template <typename Elem, typename... U>
	friend
	constexpr Elem& union_cast(union_t<U...>&);

	template <typename Elem, typename... U>
	friend
	constexpr Elem const& union_cast(union_t<U...> const&);

	template <typename Elem, typename... U>
	friend
	constexpr Elem&& union_cast(union_t<U...>&&);
};

template <typename Elem, typename... T>
inline
constexpr Elem& union_cast(union_t<T...>& value) {
	return detail::union_t::union_cast<Elem, T...>::call(value.union_);
}

template <typename Elem, typename... T>
inline
constexpr Elem const& union_cast(union_t<T...> const& value) {
	return detail::union_t::union_cast<Elem, T...>::call(value.union_);
}

template <typename Elem, typename... T>
inline
constexpr Elem&& union_cast(union_t<T...>&& value) {
	return detail::union_t::union_cast<Elem, T...>::call(std::move(value.union_));
}

}

#endif
