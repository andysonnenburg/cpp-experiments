#ifndef WART_UNDECAYED_COMMON_RESULT_OF_HPP
#define WART_UNDECAYED_COMMON_RESULT_OF_HPP

#include "result_of.hpp"
#include "undecayed_common_type.hpp"

namespace wart {

template <typename F, typename... ArgTypes>
using undecayed_common_result_of =
	undecayed_common_type<result_of_t<F, ArgTypes>...>;

template <typename F, typename... ArgTypes>
using undecayed_common_result_of_t =
	typename undecayed_common_result_of<F, ArgTypes...>::type;

}

#endif
