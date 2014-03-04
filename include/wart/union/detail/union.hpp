#ifndef WART_UNION_DETAIL_HPP
#define WART_UNION_DETAIL_HPP

#include "../../enable_if_move_constructible.hpp"

#include <type_traits>
#include <utility>

namespace wart { namespace detail { namespace union_t {

template <bool...>
struct any;

template <>
struct any<>: std::false_type {};

template <bool... Tail>
struct any<true, Tail...>: std::true_type {};

template <bool... Tail>
struct any<false, Tail...>: any<Tail...>::type {};

template <typename...>
union union_t;

template <typename T>
union union_t<T> {
	T head_;

	constexpr union_t():
		head_() {}

	constexpr union_t(T const& head):
		head_(head) {}

	constexpr union_t(T&& head):
		head_(std::move(head)) {}
};

template <typename Head, typename... Tail>
union union_t<Head, Tail...> {
	Head head_;
	union_t<Tail...> tail_;

	constexpr union_t():
		head_() {}

	constexpr union_t(Head const& head):
		head_(head) {}

	constexpr union_t(Head&& head):
		head_(std::move(head)) {}

	template <typename T>
	constexpr union_t(T const& tail):
		tail_(tail) {}

	template <typename T>
	constexpr union_t(T&& tail,
	                  typename enable_if_move_constructible<T>::type* = nullptr):
		tail_(std::move(tail)) {}
};

template <typename, typename...>
struct get;

template <typename Head, typename... Tail>
struct get<Head, Head, Tail...> {
	static constexpr Head& call(union_t<Head, Tail...>& value) {
		return value.head_;
	}

	static constexpr Head const& call(union_t<Head, Tail...> const& value) {
		return value.head_;
	}

	static constexpr Head&& call(union_t<Head, Tail...>&& value) {
		return std::move(value).head_;
	}
};

template <typename Elem, typename Head, typename... Tail>
struct get<Elem, Head, Tail...> {
	static constexpr Elem& call(union_t<Head, Tail...>& value) {
		return get<Elem, Tail...>::call(value.tail_);
	}

	static constexpr Elem const& call(union_t<Head, Tail...> const& value) {
		return get<Elem, Tail...>::call(value.tail_);
	}

	static constexpr Elem&& call(union_t<Head, Tail...>&& value) {
		return get<Elem, Tail...>::call(std::move(value).tail_);
	}
};

}}}

#endif
