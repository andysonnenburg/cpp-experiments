#ifndef WART_GRAMMAR_HPP
#define WART_GRAMMAR_HPP

#include "boost/spirit/include/qi.hpp"

#include "unique_ptr.hpp"
#include "value/syntax.hpp"

namespace wart {

template <typename Iterator>
struct grammar: boost::spirit::qi::grammar<Iterator,
                                           unique_ptr<value::syntax::Expr>(),
                                           boost::spirit::ascii::space_type> {
	grammar():
		grammar::base_type(expr) {
		// expr %= var | app | abs;
		// var %= name;
		// app %= expr >> expr;
		// abs %= '\\' >> name >> '.' >> expr;
	}

	boost::spirit::qi::rule<Iterator,
	                        unique_ptr<value::syntax::Expr>(),
	                        boost::spirit::ascii::space_type> expr;

	boost::spirit::qi::rule<Iterator,
	                        unique_ptr<value::syntax::Expr>(),
	                        boost::spirit::ascii::space_type> var;

	boost::spirit::qi::rule<Iterator,
	                        unique_ptr<value::syntax::Expr>(),
	                        boost::spirit::ascii::space_type> app;

	boost::spirit::qi::rule<Iterator,
	                        unique_ptr<value::syntax::Expr>(),
	                        boost::spirit::ascii::space_type> abs;

	boost::spirit::qi::rule<Iterator,
	                        unique_ptr<value::syntax::Expr>(),
	                        boost::spirit::ascii::space_type> name;
};

}

#endif
