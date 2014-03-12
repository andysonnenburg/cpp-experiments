#ifndef MEMORY_TRACEABLE_FOR_EACH_HPP
#define MEMORY_TRACEABLE_FOR_EACH_HPP

#include <utility>

#include "begin.hpp"

namespace memory {
	namespace traceable {
		namespace extension {
			template <typename>
			struct tag_of;

			template <typename>
			struct end;

			template <typename>
			struct deref;

			template <typename>
			struct next;

			template <typename>
			struct not_equal;
		}

		namespace detail {
			template <typename Tag>
			struct for_each {
				template <typename First, typename Last, typename F>
				static
				void call(First const& first, Last const& last, F const& f) {
					if (extension::not_equal<Tag>
							::template apply<First, Last>
							::call(first, last)) {
						f(extension::deref<Tag>::template apply<First>::call(first));
						call(extension::next<Tag>::template apply<First>::call(first), last, f);
					}
				}
			};
		}

		template <typename T, typename F>
		void for_each(T const& container, F const& f) {
			detail
				::for_each<typename extension::tag_of<T>::type>
				::call(memory::traceable::begin(container),
							 memory::traceable::end(container),
							 f);
		}
	}
}

#endif
