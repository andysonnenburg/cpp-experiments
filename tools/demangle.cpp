#include "cxxabi.h"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

std::string demangle(char const* name) {
	using namespace abi;
	using namespace std;
	int status;
	unique_ptr<char, void (*)(void*)> result(__cxa_demangle(name,
	                                                        nullptr,
	                                                        nullptr,
	                                                        &status),
	                                         free);
	switch (status) {
	case 0: return result.get();
	case -1: throw bad_alloc();
	case -2: throw logic_error("not a valid name under the C++ ABI mangling rules");
	case -3: throw invalid_argument("one of the arguments is invalid");
	}
	throw logic_error("unexpected status");
}

int main(int argc, char* argv[]) {
	using namespace std;
	if (argc != 2) {
		std::cerr << argv[0] << " " << "NAME" << std::endl;
		return 1;
	}
	cout << demangle(argv[1]) << endl;
	return 0;
}
