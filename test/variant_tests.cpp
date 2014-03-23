#include "wart/variant.hpp"

#include "gtest/gtest.h"

#include <string>
#include <type_traits>

TEST(variant, lvalue_accepts) {
	wart::variant<double, int> x(1.2);
	{
		bool visited = false;
		struct visitor {
			bool& visited_;
			void operator()(int&) const {
				FAIL();
			}
			void operator()(double& x) const {
				visited_ = true;
				EXPECT_EQ(1.2, x);
				x = 2.2;
			}
		};
		x.accept(visitor{visited});
		EXPECT_TRUE(visited);
	}
	{
		bool visited = false;
		struct visitor {
			bool& visited_;
			void operator()(int) {
				FAIL();
			}
			void operator()(double x) {
				visited_ = true;
				EXPECT_EQ(2.2, x);
			}
		};
		x.accept(visitor{visited});
		EXPECT_TRUE(visited);
	}
}

TEST(variant, lvalue_const_accepts) {
	const wart::variant<int, double> x(1);
	bool visited = false;
	struct visitor {
		bool& visited_;
		void operator()(int const& x) {
			visited_ = true;
			EXPECT_EQ(1, x);
		}
		void operator()(double const&) {
			FAIL();
		}
	};
	x.accept(visitor{visited});
	EXPECT_TRUE(visited);
}

TEST(variant, rvalue_accepts) {
	bool visited = false;
	struct visitor {
		bool& visited_;
		void operator()(int&& x) {
			visited_ = true;
			EXPECT_EQ(1, x);
		}
		void operator()(double&&) {
			FAIL();
		}
	};
	wart::variant<double, int>(1).accept(visitor{visited});
	EXPECT_TRUE(visited);
}

TEST(variant, default_constructor) {
	struct nontrivial_default_constructible {
		double value_;
		nontrivial_default_constructible():
			value_{3.14} {}
	};

	bool visited = false;
	struct visitor {
		bool& visited_;
		void operator()(nontrivial_default_constructible x) {
			visited_ = true;
			EXPECT_EQ(3.14, x.value_);
		}
		void operator()(char) {
			FAIL();
		}
	};
	wart::variant<nontrivial_default_constructible, char>{}.accept(visitor{visited});
	EXPECT_TRUE(visited);
}

TEST(variant, equality) {
	using variant = wart::variant<int, char, double, std::string>;
	EXPECT_EQ(variant{'a'}, variant{'a'});
	EXPECT_NE(variant{'a'}, variant{'b'});
	EXPECT_NE(variant{'a'}, variant{1});
}

TEST(variant, visitor_reference_result_type) {
	struct uncopyable {
		uncopyable() = default;
		uncopyable(uncopyable const&) = delete;
		uncopyable(uncopyable&&) = delete;
		~uncopyable() = default;
	};
	using variant = wart::variant<int>;
	{
		bool visited = false;
		struct visitor {
			bool& visited_;
			void operator()(int&) {
				visited_ = true;
			}
		};
		variant x{};
		x.accept(visitor{visited});
		EXPECT_TRUE(visited);
	}
	{
		bool visited = false;
		struct visitor {
			using result_type = int const&;
			bool& visited_;
			result_type operator()(int const& value) {
				visited_ = true;
				return value;
			}
		};
		const variant x{};
		EXPECT_EQ(0, x.accept(visitor{visited}));
		EXPECT_TRUE(visited);
	}
	{
		bool visited = false;
		struct visitor {
			using result_type = int&&;
			bool& visited_;
			result_type operator()(int&& value) {
				visited_ = true;
				return std::move(value);
			}
		};
		variant{}.accept(visitor{visited});
		EXPECT_TRUE(visited);
	}
}

TEST(variant, nothrow_move_constructible) {
	using is_nothrow_move_constructible =
		std::is_nothrow_move_constructible<wart::variant<int, double>>;
	EXPECT_TRUE(is_nothrow_move_constructible::value);
}
