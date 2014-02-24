#ifndef WART_MATH_HPP
#define WART_MATH_HPP

#include <cstddef>
#include <type_traits>

namespace wart {
	namespace math {
		template <typename T>
		constexpr T gcd(T const& lhs, T const& rhs) {
			return rhs == 0?
				lhs :
				gcd(rhs, lhs % rhs);
		}

		template <typename T>
		constexpr T lcm(T const& lhs, T const& rhs) {
			return lhs * rhs / gcd(lhs, rhs);
		}

		template <typename T, T Head, T... Tail>
		struct gcd_constant;

		template <typename T, T Head>
		struct gcd_constant<T, Head>: std::integral_constant<T, Head> {};

		template <typename T, T Head, T... Tail>
		struct gcd_constant:
			std::integral_constant<T, gcd(Head, gcd_constant<T, Tail...>::value)> {};

		template <typename T, T Head, T... Tail>
		struct lcm_constant;

		template <typename T, T Head>
		struct lcm_constant<T, Head>: std::integral_constant<T, Head> {};

		template <typename T, T Head, T... Tail>
		struct lcm_constant:
			std::integral_constant<T, lcm(Head, lcm_constant<T, Tail...>::value)> {};
	}
}

#endif
