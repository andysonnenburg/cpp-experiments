#ifndef WART_FOR_EACH_TUPLE_HPP
#define WART_FOR_EACH_TUPLE_HPP

#include <cstddef>
#include <tuple>

namespace wart {
	namespace extension {
		namespace for_each {
			template <typename>
			struct tag_of;

			template <typename>
			struct for_each;

			struct tuple_tag;

			template <typename... Types>
			struct tag_of<std::tuple<Types...>> {
				typedef tuple_tag type;
			};

			template <>
			struct for_each<tuple_tag> {
				template <typename>
				struct apply;

				template <typename... Types>
				struct apply<std::tuple<Types...>> {
					template <typename F>
					static void call(std::tuple<Types...> const& tuple, F&& f) {
						for_<0, sizeof...(Types), Types...>::call(tuple, std::forward<F>(f));
					}
				};

			private:
				template <std::size_t I, std::size_t N, typename... Types>
				struct for_ {
					template <typename F>
					static void call(std::tuple<Types...> const& tuple, F&& f) {
						f(std::get<I>(tuple));
						for_<I + 1, N, Types...>::call(tuple, std::forward<F>(f));
					}
				};

				template <std::size_t N, typename... Types>
				struct for_<N, N, Types...> {
					template <typename F>
					static void call(std::tuple<Types...> const&, F&&) {}
				};
			};
		}
	}
}

#endif
