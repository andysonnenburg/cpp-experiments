#include "wart/bdd/syntax.hpp"
#include "wart/grammar.hpp"
#include "wart/value/syntax.hpp"
#include "wart/variant.hpp"
#include "wart/lex/nfa.hpp"
#include "wart/lex/syntax.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main() {
	using namespace wart::bdd::syntax;

	var x1, x2, x3, x4, x5, x6;
	expr e = x1 * x2 + x3 * x4 + x5 * x6;

	// wart::bdd<6> bdd([](bdd::var x1,
	//                     bdd::var x2,
	//                     bdd::var x3,
	//                     bdd::var x4,
	//                     bdd::var x5,
	//                     bdd::var x6) {
	//	                 return x1 * x2 + x3 * x4 + x5 * x6;
	//                  });

	// bdd()

	return 0;
}
