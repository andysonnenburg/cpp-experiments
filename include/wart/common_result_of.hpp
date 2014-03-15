#ifndef WART_COMMON_RESULT_OF_HPP
#define WART_COMMON_RESULT_OF_HPP

#include <type_traits>

namespace wart {

template <typename F, typename... ArgTypes>
using common_result_of =
	std::common_type<typename std::result_of<F(ArgTypes)>::type...>;

template <typename F, typename... ArgTypes>
using common_result_of_t = typename common_result_of<F, ArgTypes...>::type;

}

#endif
