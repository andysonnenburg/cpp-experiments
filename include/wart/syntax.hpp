#ifndef WART_SYNTAX_HPP
#define WART_SYNTAX_HPP

#include "unique_ptr.hpp"
#include "variant.hpp"

#include <memory>
#include <utility>

namespace wart {

class Var;
class App;
class Abs;
class Let;
using Expr = variant<Var, App, Abs, Let>;

class Var {
	std::string x_;

public:
	Var(std::string const&);
	Var(Var&&) = default;
	~Var() = default;
	std::string& x();
	std::string const& x() const;
};

class App {
	unique_ptr<Expr> e1_;
	unique_ptr<Expr> e2_;

public:
	App() = default;
	App(Expr&&, Expr&&);
	App(App&&) = default;
	Expr& e1();
	Expr const& e1() const;
	Expr& e2();
	Expr const& e2() const;
};

class Abs {
	std::string x_;
	unique_ptr<Expr> e_;

public:
	Abs(std::string const&, Expr&&);
	Abs(Abs&&) = default;
	std::string& x();
	std::string const& x() const;
	Expr& e();
	Expr const& e() const;
};

class Let {
	std::string x_;
	unique_ptr<Expr> e1_;
	unique_ptr<Expr> e2_;

public:
	Let(std::string const&, Expr&&, Expr&&);
	Let(Let&&) = default;
	std::string& x();
	std::string const& x() const;
	Expr& e1();
	Expr const& e1() const;
	Expr& e2();
	Expr const& e2() const;
};

}

#endif
