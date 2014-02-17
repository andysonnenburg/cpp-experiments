#ifndef SHARED_CYCLIC_HPP
#define SHARED_CYCLIC_HPP

namespace shared {
	namespace cyclic {

		namespace parent {
			template <typename T>
			class traits {
			public:
				typedef typename T::children_type children_type;
			};
		}

		template <typename T>
		typename parent::traits<T>::children_type children(T const&);

		namespace detail {
			enum class color {
				black,
				gray,
				white
			};

			typedef unsigned int count;

			template <typename T>
			class counted {
			public:
				T value_;
				color color_;
				count count_;

				template <typename... Args>
				counted(Args&&... args):
					value_(std::forward<Args>(args)...),
					color_(color::black),
					count_(1) {}

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
								--t->count_;
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
							++t->count_;
							if (t->color_ == color::black) {
								t->scan_black();
							}
						}
					}
				}

				void collect_white() {
					if (color_ == color::white) {
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
			template <typename... Args>
			explicit ptr(Args&&... args):
				counted_(new detail::counted<T>(std::forward<Args>(args)...)),
				ptr_(&counted_->value_) {}

			constexpr ptr() noexcept: counted_(nullptr), ptr_() {}

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
					++counted_->count_;
				}
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

		private:
			void decrement() {
				auto counted = counted_;
				if (counted != nullptr) {
					if (--counted->count_ == 0) {
						release();
					} else {
						counted_->collect_cycles();
					}
				}
			}

			void release() {
				delete counted_;
			}

			void scan() {
				if (counted_ != nullptr) {
					counted_->scan();
				}
			}

			void collect_white() {
				if (counted_ != nullptr) {
					auto counted = counted_;
					counted_ = nullptr;
					counted->collect_white();
					delete counted;
				}
			}

			template <typename U> friend class detail::counted;
		};

		namespace parent {
			template <typename T>
			class empty {
			public:
				ptr<T>* begin() {
					return nullptr;
				}

				ptr<T>* end() {
					return nullptr;
				}
			};

			template <>
			class traits<int> {
			public:
				typedef empty<int> children_type;
			};
		}

		template <>
		parent::traits<int>::children_type children<int>(int const&) {
			return parent::empty<int>();
		}
	}
}

#endif
