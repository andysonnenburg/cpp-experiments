#ifndef WART_COPYABLE_UNIQUE_PTR_HPP
#define WART_COPYABLE_UNIQUE_PTR_HPP

#include "make_tag.hpp"

#include <memory>
#include <utility>

namespace wart {

namespace detail { namespace copyable_unique_ptr {

template <typename Allocator>
class state: Allocator {
	using allocator_traits = std::allocator_traits<Allocator>;

public:
	using allocator_type = typename allocator_traits::allocator_type;
	using value_type = typename allocator_traits::value_type;
	using pointer = typename allocator_traits::pointer;

	state():
		pointer_{nullptr} {}

	state(state const& rhs):
		pointer_{new_if_not_nullptr(allocator(), rhs.pointer_)} {}

	state(state&& rhs):
		pointer_{rhs.pointer_} {
		rhs.pointer_ = nullptr;
	}

	template <typename... Args>
	state(make_tag<value_type>, Args&&... args):
		pointer_{new_(allocator(), std::forward<Args>(args)...)} {}

	~state() {
		delete_(allocator(), pointer_);
	}

	state& operator=(state const& rhs) & {
		delete_(allocator(), pointer_);
		pointer_ = new_if_not_nullptr(allocator(), rhs.pointer_);
		return *this;
	}

	state& operator=(state&& rhs) & {
		delete_(allocator(), pointer_);
		pointer_ = rhs.pointer_;
		rhs.pointer_ = nullptr;
		return *this;
	}

	void swap(state& rhs) {
		using std::swap;
		swap(pointer_, rhs.pointer_);
	}

	pointer get() const {
		return pointer_;
	}

	pointer release() {
		pointer pointer = pointer_;
		pointer_ = nullptr;
		return pointer;
	}

	allocator_type get_allocator() {
		return allocator();
	}

private:
	pointer pointer_;

	allocator_type& allocator() {
		return *this;
	}

	allocator_type const& allocator() const {
		return *this;
	}

	static pointer new_if_not_nullptr(allocator_type& allocator, pointer rhs) {
		return rhs? new_(allocator, *rhs) : nullptr;
	}

	template <typename... Args>
	static pointer new_(allocator_type& allocator, Args&&... args) {
		auto pointer = allocator_traits::allocate(allocator, 1);
		allocator_traits::construct(allocator, pointer, std::forward<Args>(args)...);
		return pointer;
	}

	static void delete_(allocator_type& allocator, pointer pointer) {
		if (pointer) {
			allocator_traits::destroy(allocator, pointer);
			allocator_traits::deallocate(allocator, pointer, 1);
		}
	}
};

template <typename Allocator>
void swap(state<Allocator>& lhs, state<Allocator>& rhs) {
	lhs.swap(rhs);
}

}}

template <typename T, typename Allocator = std::allocator<T>>
class copyable_unique_ptr {
	using allocator_traits = typename std::allocator_traits<Allocator>::template rebind_traits<T>;

public:
	using pointer = typename allocator_traits::pointer;
	using element_type = typename std::pointer_traits<pointer>::element_type;
	using allocator_type = typename allocator_traits::allocator_type;
	using value_type = typename allocator_traits::value_type;

	copyable_unique_ptr() = default;

	template <typename... Args>
	copyable_unique_ptr(make_tag<T> tag, Args&&... args):
		state_{tag, std::forward<Args>(args)...} {}

	element_type& operator*() const {
		return *state_.get();
	}

	pointer operator->() const {
		return state_.get();
	}

	void swap(copyable_unique_ptr& rhs) {
		using std::swap;
		swap(state_, rhs.state_);
	}

	pointer get() const {
		return state_.get();
	}

	allocator_type get_allocator() {
		return state_.get_allocator();
	}

	pointer release() {
		return state_.release();
	}

private:
	detail::copyable_unique_ptr::state<allocator_type> state_;
};

template <typename T>
void swap(copyable_unique_ptr<T>& lhs, copyable_unique_ptr<T>& rhs) {
	lhs.swap(rhs);
}

}

#endif
