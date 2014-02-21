#ifndef MEMORY_TRACEABLE_FOR_EACH_HPP
#define MEMORY_TRACEABLE_FOR_EACH_HPP

namespace memory {
	namespace traceable {
		namespace extension {
			template <typename T>
			struct tag_of;

			template <typename Tag>
			struct for_each;
		}

		template <typename T, typename F>
		void for_each(T const& container, F const& f) {
			extension::for_each<typename extension::tag_of<T>::type>
				::template apply<T>
				::call(container, f);
		}
	}
}

#endif
