#ifndef WART_VARIANT_DETAIL_UNION_STORAGE_HPP
#define WART_VARIANT_DETAIL_UNION_STORAGE_HPP

#include "../../math.hpp"

#include <type_traits>

namespace wart { namespace detail { namespace variant {

template <typename... Types>
using union_storage = typename std::aligned_storage
	<math::max_constant<std::size_t,
	                    sizeof(Types)...>::value,
	 math::lcm_constant<std::size_t,
	                    std::alignment_of<Types>::value...>::value>::type;

template <typename... Types>
using union_storage_t = typename union_storage<Types...>::type;

}}}

#endif
