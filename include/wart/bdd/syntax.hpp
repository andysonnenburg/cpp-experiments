#ifndef WART_BDD_SYNTAX_HPP
#define WART_BDD_SYNTAX_HPP

#include "../make_arg.hpp"
#include "../unique_ptr.hpp"
#include "../variant.hpp"

#include <memory>
#include <type_traits>

namespace wart { namespace bdd { namespace syntax {

namespace detail {

struct Var;
struct True;
struct False;
struct And;
struct Or;
using Expr = variant<Var, True, False, And, Or>;

using Index = std::size_t;

struct Var {
	Var(): index{next_index()} {}
	Var(Var const&) = default;
	Var(Var&&) = default;

	Index const index;

	static Index next_index() {
		static Index index = 0;
		return index++;
	}
};

struct True {
	True() = default;
	True(std::true_type) {}
	operator std::true_type() {	return std::true_type(); }
};

struct False {
	False() = default;
	False(std::false_type) {}
	operator std::false_type() { return std::false_type(); }
};

struct And {
	And(And const&) = default;
	And(And&&) = default;
	template <typename Lhs, typename Rhs>
	And(Lhs&& lhs, Rhs&& rhs):
		lhs_{std::forward<Lhs>(lhs)},
		rhs_{std::forward<Rhs>(rhs)} {}

private:
	std::unique_ptr<Expr> lhs_;
	std::unique_ptr<Expr> rhs_;
};

struct Or {
	Or(Or const&) = default;
	Or(Or&&) = default;
	std::unique_ptr<Expr> lhs;
	std::unique_ptr<Expr> rhs;
};

}

class var {

private:
	detail::Var var_;

	friend
	class expr;
};

class expr {

public:
	expr(var const& arg):
		expr(make_arg_t<detail::Var>(), arg.var_) {}

	expr(bool value):
		expr_(value?
		      detail::Expr(make_arg_t<detail::True>{}) :
		      detail::Expr(make_arg_t<detail::False>{})) {}

	expr(expr const&) = default;

	expr(expr&&) = default;

private:
	template <typename T, typename... Args>
	expr(make_arg_t<T> arg, Args&&... args):
		expr_{arg, std::forward<Args>(args)...} {}

	detail::Expr expr_;

	template <typename Lhs, typename Rhs>
	friend expr operator*(Lhs&&, Rhs&&);
	template <typename Lhs, typename Rhs>
	friend expr operator+(Lhs&&, Rhs&&);
};

template <typename Lhs, typename Rhs>
inline expr operator*(Lhs&& lhs, Rhs&& rhs) {
	return expr(make_arg_t<detail::And>{},
	            make_unique<detail::Expr>(std::forward<Lhs>(lhs).expr_),
	            make_unique<detail::Expr>(std::forward<Rhs>(rhs).expr_));
}

inline expr

template <typename Lhs, typename Rhs>
inline expr operator+(Lhs&& lhs, Rhs&& rhs) {
	return expr(make_arg_t<detail::Or>{},
	            make_unique<detail::Expr>(std::forward<Lhs>(lhs).expr_),
	            make_unique<detail::Expr>(std::forward<Rhs>(rhs).expr_));
}

}}}

#endif
