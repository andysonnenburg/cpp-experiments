#ifndef WART_VALUE_SYNTAX_HPP
#define WART_VALUE_SYNTAX_HPP

#include "../unique_ptr.hpp"
#include "../variant.hpp"

#include <memory>
#include <utility>

namespace wart { namespace value { namespace syntax {

class Var;
class App;
class Abs;
using Expr = variant<Var, App, Abs>;

class Var {
	std::string x_;

public:
	template <typename String>
	Var(String&&);
	std::string& x();
	std::string const& x() const;
};

class App {
	unique_ptr<Expr> e1_;
	unique_ptr<Expr> e2_;

public:
	App(Expr&&, Expr&&);
	Expr& e1();
	Expr const& e1() const;
	Expr& e2();
	Expr const& e2() const;
};

class Abs {
	std::string x_;
	unique_ptr<Expr> e_;

public:
	template <typename String>
	Abs(String&&, Expr&&);
	Abs(Abs&&) = default;
	std::string& x();
	std::string const& x() const;
	Expr& e();
	Expr const& e() const;
};

template <typename String>
Var::Var(String&& x):
	x_(std::forward<String>(x)) {}

template <typename String>
Abs::Abs(String&& x, Expr&& e):
	x_(std::forward<String>(x)),
	e_(make_unique<Expr>(std::move(e))) {}

}}}

#endif
