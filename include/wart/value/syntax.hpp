#ifndef WART_VALUE_SYNTAX_HPP
#define WART_VALUE_SYNTAX_HPP

#include "../variant.hpp"

#include <memory>
#include <utility>

namespace wart { namespace value { namespace syntax {

class Var;
class App;
class Abs;
using Expr = wart::variant<Var, App, Abs>;

class Var {
	std::string x_;

public:
	Var(std::string const&);
	std::string& x();
	std::string const& x() const;
};

class App {
	std::shared_ptr<Expr> e1_;
	std::shared_ptr<Expr> e2_;

public:
	App(std::shared_ptr<Expr> const&, std::shared_ptr<Expr> const&);
	Expr& e1();
	Expr const& e1() const;
	Expr& e2();
	Expr const& e2() const;
};

class Abs {
	std::string x_;
	std::shared_ptr<Expr> e_;

public:
	Abs(std::string const&, std::shared_ptr<Expr> const&);
	std::string& x();
	std::string const& x() const;
	Expr& e();
	Expr const& e() const;
};

}}}

#endif
