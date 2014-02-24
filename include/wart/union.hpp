#ifndef WART_UNION_HPP
#define WART_UNION_HPP

#include <type_traits>

#include "math.hpp"

namespace wart {
	template <typename Type, typename... Types>
	using union_t = typename std::aligned_storage
		<math::max_constant<std::size_t,
												sizeof(Type),
												sizeof(Types)...>::value,
		 math::lcm_constant<std::size_t,
												std::alignment_of<Type>::value,
												std::alignment_of<Types>::value...>::value>::type;
}

#endif
