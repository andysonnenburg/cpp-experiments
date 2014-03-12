#include "wart/union.hpp"

#include "gtest/gtest.h"

TEST(union_test, union_cast_int) {
	wart::union_t<double, int> x(1);
	EXPECT_EQ(1, wart::union_cast<int>(x));
}

TEST(union_test, union_cast_double) {
	wart::union_t<double, int> x(2.1);
	EXPECT_EQ(2.1, wart::union_cast<double>(x));
}

TEST(union_test, make_union) {
	wart::union_t<double, int> x(wart::make_union_tag<int>(), 2.1);
	EXPECT_EQ(2, wart::union_cast<int>(x));
}
