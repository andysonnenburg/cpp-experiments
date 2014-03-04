#ifndef WART_VARIANT_DETAIL_ENABLE_IF_ELEM_HPP
#define WART_VARIANT_DETAIL_ENABLE_IF_ELEM_HPP

#include "elem.hpp"

#include <type_traits>

namespace wart { namespace detail { namespace variant {

template <typename Elem, typename... List>
using enable_if_elem = std::enable_if<elem<Elem, List...>::value>;

}}}

#endif
