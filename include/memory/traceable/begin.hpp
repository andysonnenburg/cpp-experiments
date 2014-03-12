#ifndef MEMORY_TRACEABLE_BEGIN_HPP
#define MEMORY_TRACEABLE_BEGIN_HPP

#include <utility>

namespace memory {
	namespace traceable {
		namespace extension {
			template <typename>
			struct tag_of;

			template <typename>
			struct begin;
		}

		namespace result_of {
			template <typename T>
			struct begin {
				typedef decltype(extension::begin<
												 typename extension::tag_of<T>::type
												 >::call(std::declval<T const&>())) type;
			};
		}

		template <typename T>
		typename result_of::begin<T>::type begin(T const& container) {
			return extension::begin<
				typename extension::tag_of<T>::type
				>::call(container);
		}
	}
}

#endif
