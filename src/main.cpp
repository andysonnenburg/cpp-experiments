#include <array>
#include "shared.hpp"

using namespace shared::cyclic;

struct link {
	typedef link const& children_type;
	ptr<link> value;
	
	struct end_iterator {
		ptr<link> operator*() const {
			return ptr<link>();
		}
		end_iterator next() const {
			return end_iterator();
		}
	};

	end_iterator end() const {
		return end_iterator();
	}

	struct value_iterator {
		ptr<link> const& value_;
		value_iterator(ptr<link> const& value): value_(value) {}
		ptr<link> const& operator*() const {
			return value_;
		}
		end_iterator next() const {
			return end_iterator();
		}
	};

	value_iterator begin() const {
		return value_iterator(value);
	}
};

bool operator!=(link::end_iterator const&, link::end_iterator const&) {
	return false;
}

bool operator!=(link::value_iterator const&, link::end_iterator const&) {
	return true;
}

int main() {
	ptr<link> x = make<link>();
	x->value = x;
	return 0;
}
