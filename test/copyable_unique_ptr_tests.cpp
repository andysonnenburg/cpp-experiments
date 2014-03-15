#include "wart/copyable_unique_ptr.hpp"
#include "wart/make_arg.hpp"

#include "gtest/gtest.h"

#include <type_traits>

TEST(copyable_unique_ptr, not_default_constructible) {
	static_assert(std::is_default_constructible<wart::copyable_unique_ptr<int>>::value,
	              "template copyable_unique_ptr must be default constructible");
}

TEST(copyable_unique_ptr, copy_constructible) {
	static_assert(std::is_copy_constructible<wart::copyable_unique_ptr<int>>::value,
	              "template copyable_unique_ptr must be copy constructible");
}

TEST(copyable_unique_ptr, move_constructible) {
	static_assert(std::is_move_constructible<wart::copyable_unique_ptr<int>>::value,
	              "template copyable_unique_ptr must be move constructible");
}

TEST(copyable_unique_ptr, construct_and_move) {
	struct uncopyable {
		uncopyable() = default;
		uncopyable(uncopyable const&) = delete;
		uncopyable(uncopyable&&) = delete;
	};
	wart::copyable_unique_ptr<uncopyable> ptr0{wart::make_arg};
	wart::copyable_unique_ptr<uncopyable> ptr1{std::move(ptr0)};
	EXPECT_EQ(nullptr, ptr0.get());
	EXPECT_NE(nullptr, ptr1.get());
}

TEST(copyable_unique_ptr, construct_and_copy) {
	struct copyable {
		copyable() = default;
		copyable(copyable const&) = default;
		copyable(copyable&&) = delete;
	};
	wart::copyable_unique_ptr<copyable> ptr0{wart::make_arg};
	wart::copyable_unique_ptr<copyable> ptr1{ptr0};
	EXPECT_NE(nullptr, ptr0.get());
	EXPECT_NE(nullptr, ptr1.get());
}
