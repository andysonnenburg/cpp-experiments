#ifndef SHARED_ACYCLIC_HPP
#define SHARED_ACYCLIC_HPP

#include <utility>

namespace shared {
	namespace acyclic {
		namespace detail {
			template <typename T, typename Count>
			class counted {
				T value_;
				Count use_count_;

			public:
				template <typename... Args>
				counted(Args&&... args):
					value_(std::forward<Args>(args)...),
					use_count_(1) {}

				T& value() {
					return value_;
				}

				T const& value() const {
					return value_;
				}

				Count& use_count() {
					return use_count_;
				}

				Count const& use_count() const {
					return use_count_;
				}
			};
		}

		template <typename T, typename Count = unsigned int>
		class ptr {
			detail::counted<T, Count>* counted_;
			T* ptr_;

		public:
			constexpr ptr() noexcept: counted_(nullptr), ptr_() {}

			template <typename U>
			ptr(ptr<U, Count> const& rhs):
				counted_(rhs.counted_),
				ptr_(rhs.ptr) {
				if (counted_ != nullptr) {
					++counted_->use_count();
				}
			}

			template <typename U>
			ptr(ptr<U, Count>&& rhs) noexcept:
				counted_(rhs.counted_),
				ptr_(rhs.ptr_) {
				rhs.counted_ = nullptr;
			}

			ptr& operator=(ptr const& rhs) {
				return operator=<T>(rhs);
			}

			template <typename U>
			ptr& operator=(ptr<U, Count> const& rhs) {
				if (counted_ == rhs.counted_) {
					return *this;
				}
				if (counted_ != nullptr && --counted_->use_count() == 0) {
					delete counted_;
				}
				counted_ = rhs.counted_;
				ptr_ = rhs.ptr_;
				if (counted_ != nullptr) {
					++counted_->use_count();
				}
				return *this;
			}

			ptr& operator=(ptr&& rhs) noexcept {
				return operator=<T>(std::move(rhs));
			}

			template <typename U>
			ptr& operator=(ptr<U>&& rhs) noexcept {
				ptr(std::move(rhs)).swap(*this);
				return *this;
			}

			~ptr() {
				if (counted_ != nullptr && --counted_->use_count() == 0) {
					delete counted_;
				}
			}
		
			typename std::add_lvalue_reference<T>::type operator*() const noexcept {
				return *ptr_;
			}

			T* operator->() const noexcept {
				return ptr_;
			}

			void swap(ptr& rhs) noexcept {
				std::swap(counted_, rhs.counted_);
				std::swap(ptr_, rhs.ptr_);
			}

		private:
			struct make_tag {};

			template <typename... Args>
			explicit ptr(make_tag, Args&&... args):
				counted_(new detail::counted<T, Count>(std::forward<Args>(args)...)),
				ptr_(&counted_->value()) {}

			friend
			bool operator==(ptr const& lhs, ptr const& rhs) noexcept {
				return lhs.counted_ == rhs.counted_;
			}
		
			friend
			bool operator!=(ptr const& lhs, ptr const& rhs) noexcept {
				return lhs.counted_ != rhs.counted_;
			}

			template <typename U, typename... Args>
			friend
			ptr<U> make(Args... args) {
				return ptr<U>(make_tag(), std::forward<Args>(args)...);
			}
		};
	}
}

namespace std {
	template <typename T>
	inline
	void swap(shared::acyclic::ptr<T>& lhs, shared::acyclic::ptr<T>& rhs) noexcept {
		lhs.swap(rhs);
	}
}

#endif
