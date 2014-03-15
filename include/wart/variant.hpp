#ifndef WART_VARIANT_HPP
#define WART_VARIANT_HPP

#include "variant/detail/variant.hpp"

#include "common_result_of.hpp"

#include <functional>
#include <utility>

namespace wart {

template <typename... T>
using variant = detail::variant::variant<T...>;

template <typename F, typename Variant>
typename F::result_type accept(F&& f, Variant&& variant) {
	return std::forward<Variant>(variant).accept(std::forward<F>(f));
}

template <
	typename F,
	typename Variant1,
	typename Variant2
>
typename F::result_type
accept(F&& f, Variant1&& variant1, Variant2&& variant2) {
	return std::forward<Variant1>(variant1)
		.accept(detail::variant::accept_variant2<F, Variant2>{
			std::forward<F>(f),
			std::forward<Variant2>(variant2)
		});
}

namespace detail { namespace variant {

template <typename... T>
inline
bool operator==(variant<T...> const& lhs, variant<T...> const& rhs) {
	return lhs.tag() == rhs.tag() && accept(equal{}, lhs, rhs);
}

template <typename... T>
inline
bool operator!=(variant<T...> const& lhs, variant<T...> const& rhs) {
	return lhs.tag() != rhs.tag() || accept(not_equal{}, lhs, rhs);
}

}}

}

namespace std {

template <typename... T>
struct hash<wart::variant<T...>> {
	using argument_type = wart::variant<T...>;
	using result_type = std::size_t;
	result_type operator()(argument_type const& arg) const {
		return arg.accept(wart::detail::variant::hash{arg.tag()});
	}
};

}

#endif
