#ifndef MEMORY_TRACEABLE_CONTAINER_HPP
#define MEMORY_TRACEABLE_CONTAINER_HPP

namespace memory {
	namespace traceable {
		namespace extension {
			template <typename>
			struct for_each;

			struct container_tag;

			template <>
			struct for_each<container_tag> {
				template <typename T>
				struct apply {
					template <typename F>
					static void call(T const& container, F const& f) {
						for (auto& element : container) {
							f(element);
						}
					}
				};
			};
		}
	}
}

#endif
