#ifndef WART_FOR_EACH_HPP
#define WART_FOR_EACH_HPP

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
	void for_each(T const& container, F const& f) {
		extension
			::for_each
			::for_each<typename extension::for_each::tag_of<T>::type>
			::template apply<T>
			::call(container, f);
	}

	template <typename T, typename F>
	void for_each(T& container, F const& f) {
		extension
			::for_each
			::for_each<typename extension::for_each::tag_of<T>::type>
			::template apply<T>
			::call(container, f);
	}
}

#endif
