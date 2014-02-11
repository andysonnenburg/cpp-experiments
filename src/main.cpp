#include <iostream>
#include "unionfind.hpp"

int main() {
	using namespace unionfind;
	ptr<int> x(1);
	ptr<int> y(2);
	ptr<int> z(3);
	y = x;
	y = z;
	std::cout << *z << std::endl;
	return 0;
}
