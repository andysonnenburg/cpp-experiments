#include "wart/grammar.hpp"
#include "wart/value/syntax.hpp"
#include "wart/variant.hpp"
#include "wart/lex/nfa.hpp"
#include "wart/lex/syntax.hpp"

#include <boost/spirit/include/qi.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main() {
	using namespace wart::lex;
	using namespace syntax;

	NFA<char> lexer{(char_('a') | char_('b')) & *char_('b')};
	lexer.lex("abc");

	using boost::spirit::ascii::space;

	std::string storage;
	std::cin.unsetf(std::ios::skipws);
	std::copy(std::istream_iterator<char>(std::cin),
	          std::istream_iterator<char>(),
	          std::back_inserter(storage));

	std::string::const_iterator i(storage.begin());
	std::string::const_iterator end(storage.end());

	wart::grammar<std::string::const_iterator> grammar;
	std::shared_ptr<wart::value::syntax::Expr> result;

	if (phrase_parse(i, end, grammar, space, result) && i == end) {
		return 0;
	}
	return 1;
}
