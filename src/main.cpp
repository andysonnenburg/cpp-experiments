#include <iostream>
#include "shared.hpp"

int main() {
	using namespace shared::cyclic;
	ptr<int> x(1);
	ptr<int> y(2);
	ptr<int> z(3);
	y = x;
	y = z;
	std::cout << *y << std::endl;
	return 0;
}
