#ifndef WART_VISITOR_TRAITS_HPP
#define WART_VISITOR_TRAITS_HPP

#include "all.hpp"
#include "has_result_of.hpp"
#include "has_result_type.hpp"
#include "undecayed_common_result_of.hpp"

#include <type_traits>

namespace wart {

namespace detail { namespace visitor_result {

template <bool HasResultType, bool HasResultOf, typename F, typename... T>
struct visitor_result;

template <bool HasResultOf, typename F, typename... T>
struct visitor_result<true, HasResultOf, F, T...> {
	using type = typename std::remove_reference<F>::type::result_type;
};

template <typename F, typename... T>
struct visitor_result<false, true, F, T...> {
	using type = undecayed_common_result_of_t<F, T...>;
};

template <typename F, typename... T>
struct visitor_result<false, false, F, T...> {};

}}

template <typename F, typename... T>
struct visitor_result: detail::visitor_result::visitor_result<
	has_result_type<typename std::remove_reference<F>::type>::value,
	all<has_result_of<F, T>::value...>::value,
	F,
	T...
	> {};

template <typename F, typename... T>
using visitor_result_t = typename visitor_result<F, T...>::type;

}

#endif
