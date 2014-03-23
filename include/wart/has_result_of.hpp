#ifndef WART_HAS_RESULT_OF_HPP
#define WART_HAS_RESULT_OF_HPP

#include "result_of.hpp"

#include <type_traits>

namespace wart {

namespace detail { namespace has_result_of {

template <typename T>
struct has_result_of {

private:
	template <typename U>
	static constexpr bool test(result_of_t<U>*) {
		return true;
	}

	template <typename...>
	static constexpr bool test(...) {
		return false;
	}

public:
	static constexpr bool value = test<T>(nullptr);
};

}}

template <typename T>
struct has_result_of: std::integral_constant<
	bool,
	detail::has_result_of::has_result_of<T>::value
	>::type {};

}

#endif
