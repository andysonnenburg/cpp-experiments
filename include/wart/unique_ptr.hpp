#ifndef WART_UNIQUE_PTR_HPP
#define WART_UNIQUE_PTR_HPP

#include <memory>

namespace wart {

template <typename T>
using unique_ptr = std::unique_ptr<T>;

template <typename T, typename... Args>
inline
unique_ptr<T> make_unique(Args&&... args) {
	return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}

#endif
