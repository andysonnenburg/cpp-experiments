#include "wart/make_tag.hpp"
#include "wart/union.hpp"

#include "gtest/gtest.h"

TEST(union_t, make_union) {
	{
		wart::union_t<double, int> x(wart::make_tag<double>(), 2.1);
		EXPECT_EQ(2.1, wart::union_cast<double>(x));
	}
	{
		wart::union_t<double, int> x(wart::make_tag<int>(), 2.1);
		EXPECT_EQ(2, wart::union_cast<int>(x));
	}
}
