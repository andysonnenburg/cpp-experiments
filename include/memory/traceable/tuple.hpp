#ifndef MEMORY_TRACEABLE_TUPLE_HPP
#define MEMORY_TRACEABLE_TUPLE_HPP

#include <tuple>

namespace memory {
	namespace traceable {
		namespace extension {
			template <typename>
			struct tag_of;

			template <typename>
			struct begin;

			template <typename>
			struct end;

			template <typename>
			struct deref;

			template <typename>
			struct next;

			template <typename>
			struct not_equal;

			struct tuple_tag;

			template <typename... Types>
			struct tag_of<std::tuple<Types...>> {
				typedef tuple_tag type;
			};

			template <std::size_t I, typename... Types>
			struct tuple_iterator {
				std::tuple<Types...> const& tuple_;
				tuple_iterator(std::tuple<Types...> const& tuple): tuple_(tuple) {}
			};

			template <>
			struct begin<tuple_tag> {
				template <typename... Types>
				static
				tuple_iterator<0, Types...> call(std::tuple<Types...> const& t) {
					return tuple_iterator<0, Types...>(t);
				}
			};

			template <>
			struct end<tuple_tag> {
				template <typename... Types>
				static
				tuple_iterator<sizeof...(Types), Types...> call(std::tuple<Types...> const& t) {
					return tuple_iterator<sizeof...(Types), Types...>(t);
				}
			};

			template <>
			struct deref<tuple_tag> {
				template <typename>
				struct apply;

				template <std::size_t I, typename... Types>
				struct apply<tuple_iterator<I, Types...>> {
					static
					typename std::tuple_element<I, std::tuple<Types...>>::type const&
					call(tuple_iterator<I, Types...> const& i) {
						return std::get<I>(i.tuple_);
					}
				};

				template <typename... Types>
				struct apply<tuple_iterator<sizeof...(Types)..., Types...>> {
					static
					typename std::tuple_element<I, std::tuple<Types...>>::type const&
					call(tuple_iterator<I, Types...> const& i) {
						assert false;
					}
				};
			};

			template <>
			struct next<tuple_tag> {
				template <typename>
				struct apply;

				template <std::size_t I, typename... Types>
				struct apply<tuple_iterator<I, Types...>> {
					static
					tuple_iterator<I + 1, Types...> call(tuple_iterator<I, Types...> const& i) {
						return tuple_iterator<I + 1, Types...>(i.tuple_);
					}
				};
			};

			template <>
			struct not_equal<tuple_tag> {
				template <typename, typename>
				struct apply;

				template <typename std::size_t I, typename... Types>
				struct apply<tuple_iterator<I, Types...>, tuple_iterator<I, Types...>> {
					static
					bool call(tuple_iterator<I, Types...> const& lhs,
										tuple_iterator<I, Types...> const& rhs) {
						return &lhs.tuple_ != &rhs.tuple_;
					}
				};

				template <typename std::size_t I, typename std::size_t J, typename... Types>
				struct apply<tuple_iterator<I, Types...>, tuple_iterator<J, Types...>> {
					static
					bool call(tuple_iterator<I, Types...> const& lhs,
										tuple_iterator<J, Types...> const& rhs) {
						return false;
					}
				};
			};
		}		
	}
}


#endif
