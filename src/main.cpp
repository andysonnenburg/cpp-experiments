#include <array>
#include "shared.hpp"

using namespace shared::cyclic;

struct link {
  typedef link const& children_type;
  ptr<link> value;
  ptr<link> const* begin() const {
    return &value;
  }
	ptr<link> const* end() const {
    return &value + 1;
  }
};

namespace shared {
	namespace cyclic {
		template <>
		children_traits<link>::children_type children<link>(link const& link) {
			return link;
		}
  }
}

int main() {
	ptr<link> x = make<link>();
  *x = link();
	x->value = x;
	return 0;
}
