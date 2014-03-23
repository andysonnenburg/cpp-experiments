#ifndef WART_HAS_RESULT_TYPE_HPP
#define WART_HAS_RESULT_TYPE_HPP

#include <type_traits>

namespace wart {

namespace detail { namespace has_result_type {

template <typename T>
struct has_result_type {

private:
	template <typename U>
	static constexpr bool check(typename std::remove_reference<
	                            typename U::result_type
	                            >::type*) {
		return true;
	}

	template <typename>
	static constexpr bool check(...) {
		return false;
	}

public:
	static constexpr bool value = check<T>(nullptr);
};

}}

template <typename T>
struct has_result_type: std::integral_constant<
	bool,
	detail::has_result_type::has_result_type<T>::value
	> {};

}

#endif
