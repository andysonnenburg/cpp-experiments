#include <iostream>
#include "shared.hpp"

int main() {
	using namespace shared;
	acyclic_ptr<int> x(1);
	acyclic_ptr<int> y(2);
	acyclic_ptr<int> z(3);
	y = x;
	y = z;
	std::cout << *y << std::endl;
	return 0;
}
