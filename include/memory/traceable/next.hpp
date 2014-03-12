#ifndef MEMORY_TRACEABLE_NEXT_HPP
#define MEMORY_TRACEABLE_NEXT_HPP

#include <utility>

namespace memory {
	namespace traceable {
		namespace extension {
			template <typename>
			struct tag_of;

			template <typename>
			struct next;
		}

		namespace result_of {
			template <typename Iterator>
			struct next {
				typedef decltype(extension::next<
												 typename extension::tag_of<Iterator>::type
												 >::call(std::declval<Iterator>())) type;
			};
		}

		template <typename Iterator>
		typename result_of::next<Iterator>::type next(Iterator&& i) {
			return extension::next<
				typename extension::tag_of<Iterator>::type
				>::call(std::forward(i));
		}
	}
}

#endif
