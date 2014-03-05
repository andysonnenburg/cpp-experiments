#include "wart/syntax.hpp"

namespace wart {

Var::Var(std::string const& x): x_(x) {}

std::string& Var::x() {
	return x_;
}

std::string const& Var::x() const {
	return x_;
}

App::App(Expr&& e1, Expr&& e2):
	e1_(make_unique<Expr>(std::move(e1))),
	e2_(make_unique<Expr>(std::move(e2))) {}

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

}
