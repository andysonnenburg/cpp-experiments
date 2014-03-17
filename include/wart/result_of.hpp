#ifndef WART_RESULT_OF_HPP
#define WART_RESULT_OF_HPP

#include <type_traits>

namespace wart {

template <typename F, typename... ArgTypes>
struct result_of: std::result_of<F(ArgTypes...)> {};

template <typename F, typename... ArgTypes>
using result_of_t = typename result_of<F, ArgTypes...>::type;

}

#endif
