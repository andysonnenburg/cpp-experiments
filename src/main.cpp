#include <array>
#include <iostream>
#include <tuple>
#include <vector>

#include "memory/traceable.hpp"

namespace memory {
	namespace traceable {
		namespace extension {
			template <typename T>
			struct tag_of<std::vector<T>> {
				typedef container_tag type;
			};
		}
	}
}

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
	return 0;
}
