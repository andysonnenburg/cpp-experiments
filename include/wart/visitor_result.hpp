#ifndef WART_VISITOR_TRAITS_HPP
#define WART_VISITOR_TRAITS_HPP

#include "all.hpp"
#include "has_result_of.hpp"
#include "undecayed_common_result_of.hpp"

#include <type_traits>

namespace wart {

namespace detail { namespace visitor_result {

template <typename Visitor>
class has_result_type {
	template <typename T>
	static constexpr bool check(typename T::result_type*) {
		return true;
	}

	template <typename>
	static constexpr bool check(...) {
		return false;
	}

public:
	static constexpr bool value = check<Visitor>(nullptr);
};

template <bool HasResultType, bool HasResultOf, typename F, typename... T>
struct visitor_result;

template <bool HasResultOf, typename F, typename... T>
struct visitor_result<true, HasResultOf, F, T...> {
	using type = typename F::result_type;
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
	detail::visitor_result::has_result_type<F>::value,
	all<has_result_of<F, T>::value...>::value,
	F,
	T...
	> {};

template <typename F, typename... T>
using visitor_result_t = typename visitor_result<F, T...>::type;

}

#endif
