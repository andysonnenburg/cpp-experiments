#ifndef WART_VARIANT_HPP
#define WART_VARIANT_HPP

#include "variant/detail/variant.hpp"

namespace wart {

template <typename... T>
using variant = detail::variant::variant<T...>;

}

#endif
