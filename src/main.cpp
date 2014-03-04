#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "wart/for_each.hpp"
#include "wart/for_each/container.hpp"
#include "wart/for_each/tuple.hpp"
#include "wart/math.hpp"
#include "wart/union.hpp"
#include "wart/variant.hpp"

struct person {
	int id;
	std::string name;
};

struct person_tag;

namespace wart {
	namespace extension {
		namespace for_each {
			template <typename T>
			struct tag_of<std::vector<T>> {
				typedef container_tag type;
			};

			template <>
			struct tag_of<person> {
				typedef person_tag type;
			};

			template <>
			struct for_each<person_tag> {
				template <typename>
				struct apply;
			};

			template <>
			struct for_each<person_tag>::apply<person> {
				template <typename F>
				static void call(person const& person, F const& f) {
					f(person.id);
					f(person.name);
				}
			};
		}
	}
}

struct print {
	template <typename T>
	void operator()(T const& x) const {
		std::cout << x << std::endl;
	}
};

constexpr wart::variant<int> i(1);

struct test {
	test() = default;
	test(test const&) noexcept {
		std::cout << "copied" << std::endl;
	}
	test(test&&) noexcept {
		std::cout << "moved" << std::endl;
	}
	test& operator=(test const&) & {
		std::cout << "copy-assigned" << std::endl;
		return *this;
	}
	test& operator=(test&&) & {
		std::cout << "move-assigned" << std::endl;
		return *this;
	}
};

struct print_visitor {
	void operator()(std::string const& x) const {
		std::cout << "string(" << x << ")" << std::endl;
	}
	void operator()(int const& x) const {
		std::cout << "int(" << x << ")" << std::endl;
	}
	void operator()(double const& x) const {
		std::cout << "double(" << x << ")" << std::endl;
	}
	void operator()(test const&) const {
		std::cout << "test()" << std::endl;
	}
};

struct sfinae_test {
	sfinae_test() {}
	sfinae_test(sfinae_test&) {
		std::cout << "meh" << std::endl;
	}
	sfinae_test(sfinae_test const&) {
		std::cout << "yay" << std::endl;
	}
	template <typename T>
	sfinae_test(T&&) {
		std::cout << "boo" << std::endl;
	}
};

int main() {
	using namespace wart;

	const union_t<int, char, double> x(2.1);
	double const& i = union_get<double>(x);
	std::cout << i << std::endl;
	std::cout << union_get<double>(union_t<int, double>(2.1)) << std::endl;

	variant<int, double> value{1};
	variant<double, int> with_double{1.0};
	value = with_double;
	const print_visitor f{};
	value.accept(f);
	with_double.accept(f);
	variant<int, double> other = value;
	other.accept(f);
	other.accept(print_visitor());
	variant<test, double> other2 = variant<test>(test());
	variant<test, double, int> other3 = other2;
	other2.accept(print_visitor());
	other2 = variant<test>(test());

	// variant<int> x(1);
	// variant<int> y(2);
	// accept(x, y, [](int x, int y) {
	//		std::cout << x << " " << y << std::endl;
	//	});

	std::vector<int> xs { 1, 2, 3 };
	for_each(xs, [](int x) {
			std::cout << x << std::endl;
		});
	std::tuple<int, int, int> ys { 1, 2, 3 };
	for_each(ys, [](int y) {
			std::cout << y << std::endl;
		});
	person person { 1, "andy" };
	for_each(person, print());

	std::cout << math::lcm_constant<std::size_t, 28, 7, 42>::value << std::endl;

	return 0;
}
