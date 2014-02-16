#ifndef SHARED_HPP
#define SHARED_HPP

#include <utility>

namespace shared {
	namespace detail {
		template <typename T, typename Count>
		class counted {
			Count count_;
			T value_;

		public:
			template <typename... Args>
			counted(Args&&... args): count_(1), value_(std::forward<Args>(args)...) {}

			void acquire() {
				++count_;
			}
			
			Count& release() {
				return --count_;
			}

			T& get() {
				return value_;
			}
		};
	}

	template <typename T, typename Count = unsigned int>
	class cyclic_ptr {
		detail::counted<T, Count>* counted_;
		T* ptr_;

	public:
		
	};

	template <typename T, typename Count = unsigned int>
	class acyclic_ptr {
		detail::counted<T, Count>* counted_;
		T* ptr_;

	public:
		template <typename... Args>
		explicit acyclic_ptr(Args&&... args):
			counted_(new detail::counted<T, Count>(std::forward<Args>(args)...)),
			ptr_(&counted_->get()) {}

		constexpr acyclic_ptr(): counted_(0), ptr_() {}
		
		template <typename U>
		acyclic_ptr(acyclic_ptr<U> const& rhs):
			counted_(rhs.counted_),
			ptr_(rhs.ptr) {
			if (counted_) {
				counted_->acquire();
			}
		}

		template <typename U>
		acyclic_ptr(acyclic_ptr<U>&& rhs) noexcept:
			counted_(rhs.counted_),
			ptr_(rhs.ptr_) {
			rhs.counted_ = 0;
		}

		acyclic_ptr& operator=(acyclic_ptr const& rhs) {
			return operator=<T>(rhs);
		}

		template <typename U>
		acyclic_ptr& operator=(acyclic_ptr<U> const& rhs) {
			if (counted_ == rhs.counted_) {
				return *this;
			}
			if (counted_ && counted_->release() == 0) {
				delete counted_;
			}
			counted_ = rhs.counted_;
			ptr_ = rhs.ptr_;
			if (counted_) {
				counted_->acquire();
			}
			return *this;
		}

		acyclic_ptr& operator=(acyclic_ptr&& rhs) {
			return operator=<T>(std::forward<T>(rhs));
		}

		template <typename U>
		acyclic_ptr& operator=(acyclic_ptr<U>&& rhs) {
			if (counted_ == rhs.counted_) {
				return *this;
			}
			if (counted_ && counted_->release() == 0) {
				delete counted_;
			}
			counted_ = rhs.counted_;
			ptr_ = rhs.ptr_;
			rhs.counted_ = 0;
			return *this;
		}

		~acyclic_ptr() {
			if (counted_ && counted_->release() == 0) {
				delete counted_;
			}
		}
		
		typename std::add_lvalue_reference<T>::type operator*() const noexcept {
			return *ptr_;
		}

		T* operator->() const noexcept {
			return ptr_;
		}
	};
}

#endif
