#include <array>
#include <iostream>
#include <tuple>
#include <vector>

#include "memory/traceable.hpp"

struct person {
	int id;
	std::string name;
};

struct person_tag;

namespace memory {
	namespace traceable {
		namespace extension {
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
				template <typename T>
				struct apply {
					template <typename F>
					static void call(T const& person, F const& f) {
						f(person.id);
						f(person.name);
					}
				};
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

int main() {
	using namespace memory::traceable;

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
	return 0;
}
