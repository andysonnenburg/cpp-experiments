#ifndef WART_RESULT_OF_HPP
#define WART_RESULT_OF_HPP

#include "declval.hpp"

namespace wart {

template <typename F, typename... ArgTypes>
struct result_of {
	using type = decltype(declval<F>()(declval<ArgTypes>()...));
};

template <typename F, typename... ArgTypes>
using result_of_t = typename result_of<F, ArgTypes...>::type;

}

#endif
