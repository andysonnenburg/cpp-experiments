#ifndef SHARED_CYCLIC_HPP
#define SHARED_CYCLIC_HPP

#include <type_traits>

namespace shared {
	namespace cyclic {
		template <typename T>
		struct children_traits {
			typedef typename T::children_type children_type;
		};

		template <typename T>
		typename children_traits<T>::children_type children(T const&);

		template <typename T>
		struct is_cyclic: std::true_type {};

		namespace detail {
			typedef unsigned int count;

			enum class color {
				green,
				black,
				gray,
				white
			};

			template <typename T, bool Cyclic = is_cyclic<T>::value>
			class counted;

			template <typename T>
			class counted<T, false> {
				T value_;
				count use_count_;

			public:
				template <typename... Args>
				counted(Args&&... args):
					value_ { std::forward<Args>(args)... },
					use_count_ { 1 } {}

				T& value() {
					return value_;
				}

				T const& value() const {
					return value_;
				}

				void increment() {
					++use_count_;
				}

				void decrement() {
					if (--use_count_ == 0) {
						delete this;
					}
				}

				void mark_cyclic() {}
				void scan() {}
				void scan_black() {}
				void collect_white() {}
			};

			template <typename T>
			class counted<T, true> {
				T value_;
				count use_count_;
				color color_;

			public:
				template <typename... Args>
				counted(Args&&... args):
					value_ { std::forward<Args>(args)... },
					use_count_ { 1 },
					color_ { color::green } {}

				T& value() {
					return value_;
				}

				T const& value() const {
					return value_;
				}

				void increment() {
					++use_count_;
				}

				void decrement() {
					if (--use_count_ == 0) {
						delete this;
					} else if (color_ != color::green) {
						collect_cycles();
					}
				}

				void mark_cyclic() {
					if (color_ == color::green) {
						color_ = color::black;
						for (auto& ptr: children(value_)) {
							ptr.mark_cyclic();
						}
					}
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
								--t->use_count_;
								t->mark_gray();
							}
						}
					}
				}

				void scan() {
					if (color_ == color::gray) {
						if (use_count_ > 0) {
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
							++t->use_count_;
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
						delete this;
					}
				}
			};
		}

		template <typename T>
		class ptr {
			detail::counted<T>* counted_;
			T* ptr_;

		public:
			constexpr ptr() noexcept: counted_ { nullptr }, ptr_ {} {}

			template <typename U>
			ptr(ptr<U> const& rhs):
				counted_ { rhs.counted_ },
				ptr_ { rhs.ptr_ } {
					increment();
			}

			template <typename U>
			ptr(ptr<U>&& rhs) noexcept:
				counted_ { rhs.counted_ },
				ptr_ { rhs.ptr_ } {
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
					counted_->increment();
					counted_->mark_cyclic();
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

		private:
			struct make_tag {};

			template <typename... Args>
			ptr(make_tag, Args&&... args):
				counted_ { new detail::counted<T> { std::forward<Args>(args)... } },
				ptr_ { &counted_->value() } {}

			void increment() const {
				if (counted_ != nullptr) {
					counted_->increment();
				}
			}

			void decrement() const {
				if (counted_ != nullptr) {
					counted_->decrement();
				}
			}

			void mark_cyclic() const {
				if (counted_ != nullptr) {
					counted_->mark_cyclic();
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
				}
			}

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
			ptr<U> make(Args... args);

			template <typename U> friend class detail::counted;
		};

		template <typename U, typename... Args>
		inline
		ptr<U> make(Args... args) {
			return ptr<U>(typename ptr<U>::make_tag {}, std::forward<Args>(args)...);
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
