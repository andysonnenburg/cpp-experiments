#ifndef MEMORY_TRACEABLE_CONTAINER_HPP
#define MEMORY_TRACEABLE_CONTAINER_HPP

#include <utility>

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

			struct container_tag;

			template <>
			struct begin<container_tag> {
				template <typename T>
				static
				decltype(std::begin(std::declval<T const&>())) call(T const& container) {
					return std::begin(container);
				}
			};

			template <>
			struct end<container_tag> {
				template <typename T>
				static
				decltype(std::end(std::declval<T const&>())) call(T const& container) {
					return std::end(container);
				}
			};

			template <>
			struct deref<container_tag> {
				template <typename Iterator>
				struct apply {
					static
					decltype(*std::declval<Iterator const&>()) call(Iterator const& i) {
						return *i;
					}
				};
			};

			template <>
			struct next<container_tag> {
				template <typename Iterator>
				struct apply {
					static
					decltype(std::declval<Iterator const&>() + 1) call(Iterator const& i) {
						return i + 1;
					}
				};
			};

			template <>
			struct not_equal<container_tag> {
				template <typename, typename>
				struct apply;

				template <typename Iterator>
				struct apply<Iterator, Iterator> {
					static
					bool call(Iterator const& lhs, Iterator const& rhs) {
						return lhs != rhs;
					}
				};
			};
		}		
	}
}

#endif
