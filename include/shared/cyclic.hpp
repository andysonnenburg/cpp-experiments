#ifndef SHARED_CYCLIC_HPP
#define SHARED_CYCLIC_HPP

#include <array>

namespace shared {
	namespace cyclic {
		template <typename T>
		class children_traits {
		public:
			typedef typename T::children_type children_type;
		};

		template <typename T>
		typename children_traits<T>::children_type children(T const&);

		namespace detail {
			enum class color {
				black,
				gray,
				white
			};

			typedef unsigned int count;

			template <typename T>
			class counted {
				T value_;
				color color_;
				count count_;

			public:
				template <typename... Args>
				counted(Args&&... args):
					value_(std::forward<Args>(args)...),
					color_(color::black),
					count_(1) {}

				T& value() {
					return value_;
				}

				T const& value() const {
					return value_;
				}

				count& use_count() {
					return count_;
				}

				count const& use_count() const {
					return count_;
				}

				void collect_cycles() {
					mark_gray();
					scan();
					collect_white();
				}

				void mark_gray() {
					if (color_ != color::gray) {
						color_ = color::gray;
						for (auto& ptr: children(value_)) {
							auto t = ptr.counted_;
							if (t != nullptr) {
								--t->use_count();
								t->mark_gray();
							}
						}
					}
				}

				void scan() {
					if (color_ == color::gray) {
						if (count_ > 0) {
							scan_black();
						} else {
							color_ = color::white;
							for (auto& ptr: children(value_)) {
								ptr.scan();
							}
						}
					}
				}

				void scan_black() {
					color_ = color::black;
					for (auto& ptr: children(value_)) {
						auto t = ptr.counted_;
						if (t != nullptr) {
							++t->use_count();
							if (t->color_ != color::black) {
								t->scan_black();
							}
						}
					}
				}

				void collect_white() {
					if (color_ == color::white) {
						color_ = color::black;
						for (auto& ptr: children(value_)) {
							ptr.collect_white();
						}
					}
				}
			};
		}

		template <typename T>
		class ptr {
			detail::counted<T>* counted_;
			T* ptr_;

		public:
			constexpr ptr() noexcept: counted_(nullptr), ptr_() {}

			template <typename U>
			ptr(ptr<U> const& rhs):
				counted_(rhs.counted_),
				ptr_(rhs.ptr_) {
				if (counted_ != nullptr) {
					++counted_->use_count();
				}
			}

			template <typename U>
			ptr(ptr<U>&& rhs) noexcept:
				counted_(rhs.counted_),
				ptr_(rhs.ptr_) {
				rhs.counted_ = nullptr;
			}

			ptr& operator=(ptr const& rhs) {
				return operator=<T>(rhs);
			}

			template <typename U>
			ptr& operator=(ptr<U> const& rhs) {
				if (counted_ == rhs.counted_) {
					return *this;
				}
				decrement();
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
				decrement();
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

			friend
			inline
			bool operator==(ptr const& lhs, ptr const& rhs) noexcept {
				return lhs.counted_ == rhs.counted_;
			}

			friend
			inline
			bool operator!=(ptr const& lhs, ptr const& rhs) noexcept {
				return lhs.counted_ != rhs.counted_;
			}

		private:
			struct make_tag {};

			template <typename... Args>
			ptr(make_tag, Args&&... args):
				counted_(new detail::counted<T>(std::forward<Args>(args)...)),
				ptr_(&counted_->value()) {}

			void decrement() const {
				auto counted = counted_;
				if (counted != nullptr) {
					if (--counted->use_count() == 0) {
						delete counted;
					} else {
						counted->collect_cycles();
					}
				}
			}

			void scan() const {
				if (counted_ != nullptr) {
					counted_->scan();
				}
			}

			void collect_white() const {
				if (counted_ != nullptr) {
					counted_->collect_white();
					delete counted_;
				}
			}

			template <typename U> friend class detail::counted;
			template <typename U, typename... Args>
			friend
			inline
			ptr<U> make(Args... args) {
				return ptr<U>(make_tag(), std::forward<Args>(args)...);
			}
		};

		template <>
		class children_traits<int> {
		public:
			typedef std::array<int, 0> children_type;
		};

		template <>
		children_traits<int>::children_type children<int>(int const&) {
			return std::array<int, 0>();
		}
	}
}

namespace std {
	template <typename T>
	inline
	void swap(shared::cyclic::ptr<T>& lhs, shared::cyclic::ptr<T>& rhs) noexcept {
		lhs.swap(rhs);
	}
}

#endif
