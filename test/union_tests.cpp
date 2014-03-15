#include "wart/make_arg.hpp"
#include "wart/union.hpp"

#include "gtest/gtest.h"

TEST(union_t, make_union) {
	{
		wart::union_t<double, int> x(wart::make_arg_t<double>{}, 2.1);
		EXPECT_EQ(2.1, wart::union_cast<double>(x));
	}
	{
		wart::union_t<double, int> x(wart::make_arg_t<int>{}, 2.1);
		EXPECT_EQ(2, wart::union_cast<int>(x));
	}
}
