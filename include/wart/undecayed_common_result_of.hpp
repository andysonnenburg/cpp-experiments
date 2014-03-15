#ifndef WART_UNDECAYED_COMMON_RESULT_OF_HPP
#define WART_UNDECAYED_COMMON_RESULT_OF_HPP

#include "undecayed_common_type.hpp"

#include <type_traits>

namespace wart {

template <typename F, typename... ArgTypes>
using undecayed_common_result_of =
	undecayed_common_type<typename std::result_of<F(ArgTypes)>::type...>;

template <typename F, typename... ArgTypes>
using undecayed_common_result_of_t =
	typename undecayed_common_result_of<F, ArgTypes...>::type;

}

#endif
