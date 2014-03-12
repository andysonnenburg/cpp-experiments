#include "wart/union.hpp"

#include "gtest/gtest.h"

TEST(union_test, make_union) {
	wart::union_t<double, int> x(1);
	EXPECT_EQ(1, wart::union_cast<int>(x));
}
