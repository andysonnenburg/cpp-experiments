#ifndef WART_MAKE_ARG_HPP
#define WART_MAKE_ARG_HPP

namespace wart {

template <typename T = void>
struct make_arg_t {};

constexpr make_arg_t<> make_arg = make_arg_t<void>{};

}

#endif
