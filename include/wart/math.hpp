#ifndef WART_MATH_HPP
#define WART_MATH_HPP

#include <cstddef>
#include <type_traits>

namespace wart {
	namespace math {
		template <std::size_t, std::size_t>
		struct gcd;

		template <std::size_t A>
		struct gcd<A, 0>: public std::integral_constant<std::size_t, A> {};

		template <std::size_t A, std::size_t B>
		struct gcd: public gcd<B, A % B> {};

		template <std::size_t A, std::size_t B>
		struct lcm: std::integral_constant<std::size_t, A * B / gcd<A, B>::value> {};
	}
}

#endif
