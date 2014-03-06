#include "gtest/gtest.h"

#include "wart/variant.hpp"

TEST(variant_test, lvalue_accepts) {
	wart::variant<int, double> x(1);
	struct {
		void operator()(int x) {
			EXPECT_EQ(1, x);
		}
		void operator()(double) {
			FAIL();
		}
	} visitor;
	x.accept(visitor);
}

TEST(variant_test, lvalue_const_accepts) {
	const wart::variant<int, double> x(1);
	struct {
		void operator()(int x) {
			EXPECT_EQ(1, x);
		}
		void operator()(double) {
			FAIL();
		}
	} visitor;
	x.accept(visitor);
}

TEST(variant_test, rvalue_accepts) {
	struct {
		void operator()(int x) {
			EXPECT_EQ(1, x);
		}
		void operator()(double) {
			FAIL();
		}
	} visitor;
	wart::variant<int, double>(1).accept(visitor);
}
