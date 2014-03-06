#ifndef WART_GRAMMAR_HPP
#define WART_GRAMMAR_HPP

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include "union.hpp"
#include "unique_ptr.hpp"
#include "value/syntax.hpp"

#include <memory>

namespace wart {

using attribute = std::shared_ptr<value::syntax::Expr>;

template <typename Iterator>
struct grammar: boost::spirit::qi::grammar<Iterator,
                                           attribute(),
                                           boost::spirit::ascii::space_type> {
	grammar():
		grammar::base_type(expr) {
		using namespace wart::value::syntax;
		using namespace boost::phoenix;
		using namespace boost::spirit::qi;

		expr
			= parens
			| var
			| abs
			;

		parens
			= ('(' >> expr >> ')')
			;

		var
			= name
			[ _val = construct<attribute>(new_<Expr>(construct<Var>(_1))) ]
			;

		abs
			= ('\\' >> name >> '.' >> expr)
			[ _val = construct<attribute>(new_<Expr>(construct<Abs>(_1, _2))) ]
			;

		name %= lexeme[char_("a-zA-Z_") >> *char_("a-zA-Z_0-9")];
	}

	boost::spirit::qi::rule<Iterator,
	                        attribute(),
	                        boost::spirit::ascii::space_type> expr, apps, parens, var, abs;

	boost::spirit::qi::rule<Iterator,
	                        std::string(),
	                        boost::spirit::ascii::space_type> name;
};

}

#endif
