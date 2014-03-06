#include "wart/value/syntax.hpp"

#include <memory>

namespace wart { namespace value { namespace syntax {

Var::Var(std::string const& x):
	x_{x} {}

std::string& Var::x() {
	return x_;
}

std::string const& Var::x() const {
	return x_;
}

App::App(std::shared_ptr<Expr> const& e1,
         std::shared_ptr<Expr> const& e2):
	e1_{e1},
	e2_{e2} {}

Expr& App::e1() {
	return *e1_;
}

Expr const& App::e1() const {
	return *e1_;
}

Expr& App::e2() {
	return *e2_;
}

Expr const& App::e2() const {
	return *e2_;
}

Abs::Abs(std::string const& x, std::shared_ptr<Expr> const& e):
	x_{x},
	e_{e} {}

std::string& Abs::x() {
	return x_;
}

std::string const& Abs::x() const {
	return x_;
}

Expr& Abs::e() {
	return *e_;
}

Expr const& Abs::e() const {
	return *e_;
}

}}}
