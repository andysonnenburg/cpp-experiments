#include <array>
#include <iostream>
#include "shared.hpp"

using namespace shared::cyclic;

struct link {
	ptr<link> value;
};

namespace shared {
	namespace cyclic {
		namespace parent {
			template <>
			class traits<link> {
			public:
				typedef std::array<ptr<link>, 1> children_type;
			};
		}

		template <>
		parent::traits<link>::children_type children<link>(link const& link) {
			return { { link.value } };
		}
	}
}

int main() {
	ptr<link> x;
	x->value = x;
	return 0;
}
