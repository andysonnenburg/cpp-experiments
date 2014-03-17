#include "wart/has_result_type.hpp"

#include "gtest/gtest.h"

TEST(has_result_type, lvalue) {
	struct F {
		using result_type = int&;
	};
	static_assert(wart::has_result_type<F>::value, "");
}

TEST(has_result_type, lvalue_reference) {
	struct F {
		using result_type = int&;
	};
	static_assert(wart::has_result_type<F&>::value, "");
}

TEST(has_result_type, const_lvalue_reference) {
	struct F {
		using result_type = int&;
	};
	static_assert(wart::has_result_type<const F&>::value, "");
}

TEST(has_result_type, rvalue_reference) {
	struct F {
		using result_type = int&;
	};
	static_assert(wart::has_result_type<F&&>::value, "");
}
