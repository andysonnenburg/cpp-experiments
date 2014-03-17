#ifndef WART_HAS_RESULT_OF_HPP
#define WART_HAS_RESULT_OF_HPP

#include "result_of.hpp"

#include <type_traits>

namespace wart {

namespace detail { namespace has_result_of {

template <typename F, typename... ArgTypes>
class has_result_of {

	template <typename A>
	static constexpr bool check(decltype(declval<A>()(declval<ArgTypes>()...))*) {
		return true;
	}

	template <typename>
	static constexpr bool check(...) {
		return false;
	}

public:
	static constexpr bool value = check<F>(nullptr);
};

}}

template <typename F, typename... ArgTypes>
struct has_result_of: std::integral_constant<
	bool,
	detail::has_result_of::has_result_of<F, ArgTypes...>::value
	>::type {};

}

#endif
