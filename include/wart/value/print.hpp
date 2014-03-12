#ifndef WART_VALUE_PRINT_HPP
#define WART_VALUE_PRINT_HPP

#include "syntax.hpp"

class Pretty {
	Expr const& expr_;

public:
	Pretty(Expr const& expr): expr_(expr) {}
};



#endif
