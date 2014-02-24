#ifndef WART_FOR_EACH_CONTAINER_HPP
#define WART_FOR_EACH_CONTAINER_HPP

namespace wart {
	namespace extension {
		namespace for_each {
			template <typename>
			struct for_each;

			struct container_tag;

			template <>
			struct for_each<container_tag> {
				template <typename T>
				struct apply {
					template <typename F>
					static void call(T const& container, F const& f) {
						for (auto const& element : container) {
							f(element);
						}
					}
				};
			};
		}
	}
}

#endif
