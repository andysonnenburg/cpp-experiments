#ifndef WART_FOR_EACH_HPP
#define WART_FOR_EACH_HPP

#include <utility>

namespace wart {
	namespace extension {
		namespace for_each {
			template <typename T>
			struct tag_of;

			template <typename Tag>
			struct for_each;
		}
	}

	template <typename T, typename F>
	void for_each(T&& container, F&& f) {
		extension
			::for_each
			::for_each<typename extension::for_each::tag_of<typename std::remove_reference<T>::type>::type>
			::template apply<typename std::remove_reference<T>::type>
			::call(std::forward<T>(container), std::forward<F>(f));
	}
}

#endif
