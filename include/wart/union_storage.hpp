#ifndef WART_UNION_HPP
#define WART_UNION_HPP

#include <type_traits>

#include "math.hpp"

namespace wart {
	namespace detail {
		namespace union_storage {
			template <typename Elem, typename... List>
			struct elem;

			template <typename Head, typename... Tail>
			struct elem<Head, Head, Tail...>: std::true_type {};

			template <typename Elem, typename Head, typename... Tail>
			struct elem<Elem, Head, Tail...>: elem<Elem, Tail...>::type {};

			template <typename Elem>
			struct elem<Elem>: std::false_type {};
		}
	}

	template <typename... Types>
	using union_storage = typename std::aligned_storage
		<math::max_constant<std::size_t,
		                    sizeof(Types)...>::value,
		 math::lcm_constant<std::size_t,
		                    std::alignment_of<Types>::value...>::value>::type;

	template <typename T, typename... Types>
	inline
	T const& union_cast(union_storage<Types...> const& value) {
		using namespace detail::union_storage;
		static_assert(elem<T, Types...>::value,
		              "template alias union_storage can only be cast to one of the template arguments Types");
		return *static_cast<T const*>(static_cast<void const*>(&value));
	}

	template <typename T, typename... Types>
	inline
	T& union_cast(union_storage<Types...>& value) {
		using namespace detail::union_storage;
		static_assert(elem<T, Types...>::value,
		              "template alias union_storage can only be cast to one of the template arguments Types");
		return *static_cast<T*>(static_cast<void*>(&value));
	}

	template <typename T, typename... Types>
	inline
	T&& union_cast(union_storage<Types...>&& value) {
		using namespace detail::union_storage;
		static_assert(elem<T, Types...>::value,
		              "template alias union_storage can only be cast to one of the template arguments Types");
		return std::forward<T>(*static_cast<T*>(static_cast<void*>(&value)));
	}
}

#endif
