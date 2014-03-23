#ifndef WART_RESULT_OF_HPP
#define WART_RESULT_OF_HPP

#include <type_traits>

namespace wart {

template <typename... Args>
using result_of_t = typename std::result_of<Args...>::type;

}

#endif
