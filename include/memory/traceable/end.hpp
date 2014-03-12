#ifndef MEMORY_TRACEABLE_END_HPP
#define MEMORY_TRACEABLE_END_HPP

#include <utility>

namespace memory {
	namespace traceable {
		namespace extension {
			template <typename>
			struct tag_of;

			template <typename>
			struct end;
		}

		namespace result_of {
			template <typename T>
			struct end {
				typedef decltype(extension::end<
												 typename extension::tag_of<T>::type
												 >::call(std::declval<T const&>())) type;
			};
		}

		template <typename T>
		typename result_of::end<T>::type end(T const& container) {
			return extension::end<
				typename extension::tag_of<T>::type
				>::call(container);
		}
	}
}

#endif
