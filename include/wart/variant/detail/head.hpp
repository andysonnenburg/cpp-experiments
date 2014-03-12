#ifndef WART_VARIANT_DETAIL_HEAD_HPP
#define WART_VARIANT_DETAIL_HEAD_HPP

namespace wart { namespace detail { namespace variant {

template <typename...>
struct head;

template <typename Head, typename... Tail>
struct head<Head, Tail...> {
	typedef Head type;
};

}}}

#endif
