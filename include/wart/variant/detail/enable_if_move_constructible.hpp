#ifndef WART_VARIANT_DETAIL_ENABLE_IF_MOVE_CONSTRUCTIBLE_HPP
#define WART_VARIANT_DETAIL_ENABLE_IF_MOVE_CONSTRUCTIBLE_HPP

#include <type_traits>

namespace wart { namespace detail { namespace variant {

template <typename T, typename U = void>
using enable_if_move_constructible =
	std::enable_if<std::is_move_constructible<T>::value, U>;

}}}

#endif
