#ifndef SHARED_CYCLIC_HPP
#define SHARED_CYCLIC_HPP

#include <utility>

namespace shared {
	namespace cyclic {
		namespace result_of {
			template <typename T>
			struct begin {
				typedef decltype(std::declval<T>().begin()) type;
			};
		}

		namespace call {
			template <typename T>
			struct begin {
				static typename result_of::begin<T>::type call(T const& seq) {
					return seq.begin();
				}
			};
		}

		template <typename T>
		typename result_of::begin<T>::type begin(T const& seq) {
			return call::begin<T>::call(seq);
		}

		namespace result_of {
			template <typename T>
			struct end {
				typedef decltype(std::declval<T>().end()) type;
			};
		}

		namespace call {
			template <typename T>
			struct end {
				static typename result_of::end<T>::type call(T const& seq) {
					return seq.end();
				}
			};
		}

		template <typename T>
		typename result_of::end<T>::type end(T const& seq) {
			return seq.end();
		}

		namespace result_of {
			template <typename Iterator>
			struct next {
				typedef decltype(std::declval<Iterator>().next()) type;
			};
		}

		namespace call {
			template <typename Iterator>
			struct next {
				static typename result_of::next<Iterator>::type call(Iterator const& i) {
					return i.next();
				}
			};
		}

		template <typename Iterator>
		typename result_of::next<Iterator>::type next(Iterator const& i) {
			return call::next<Iterator>::call(i);
		}

		namespace result_of {
			template <typename Iterator>
			struct deref {
				typedef decltype(*std::declval<Iterator>()) type;
			};
		}

		namespace call {
			template <typename Iterator>
			struct deref {
				static typename result_of::deref<Iterator>::type call(Iterator const& i) {
					return *i;
				}
			};
		}

		template <typename Iterator>
		typename result_of::deref<Iterator>::type deref(Iterator const& i) {
			return call::deref<Iterator>::call(i);
		}

		namespace result_of {
			template <typename T, typename U>
			struct not_equal {
				typedef decltype(std::declval<T>() != std::declval<U>()) type;
			};
		}

		namespace call {
			template <typename T, typename U>
			struct not_equal {
				static typename result_of::not_equal<T, U>::type call(T const& t, U const& u) {
					return t != u;
				}
			};
		}

		template <typename T, typename U>
		typename result_of::not_equal<T, U>::type not_equal(T const& t, U const& u) {
			return call::not_equal<T, U>::call(t, u);
		}

		template <typename T>
		struct is_acyclic: std::false_type {};

		namespace detail {
			using namespace shared::cyclic;

			typedef unsigned int count;

			enum class color {
				green,
				black,
				gray,
				white
			};

			template <typename T, bool Acyclic = is_acyclic<T>::value>
			class counted;

			template <typename T>
			class counted<T, true> {
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
			class counted<T, false> {
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
						mark_cyclic(begin(value_), end(value_));
					}
				}

			private:
				template <typename First, typename Last>
				static
				void mark_cyclic(First const& first, Last const& last) {
					if (not_equal(first, last)) {
						deref(first).mark_cyclic();
						mark_cyclic(next(first), last);
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
						mark_gray(begin(value_), end(value_));
					}
				}

				template <typename First, typename Last>
				static
				void mark_gray(First const& first, Last const& last) {
					if (not_equal(first, last)) {
						auto t = deref(first).counted_;
						if (t != nullptr) {
							--t->use_count_;
							t->mark_gray();
						}
						mark_gray(next(first), last);
					}
				}

			public:
				void scan() {
					if (color_ == color::gray) {
						if (use_count_ > 0) {
							scan_black();
						} else {
							color_ = color::white;
							scan(begin(value_), end(value_));
						}
					}
				}

			private:
				template <typename First, typename Last>
				static
				void scan(First const& first, Last const& last) {
					if (not_equal(first, last)) {
						deref(first).scan();
						scan(next(first), last);
					}
				}

				void scan_black() {
					color_ = color::black;
					scan_black(begin(value_), end(value_));
				}

				template <typename First, typename Last>
				static
				void scan_black(First const& first, Last const& last) {
					if (not_equal(first, last)) {
						auto t = deref(first).counted_;
						if (t != nullptr) {
							++t->use_count_;
							if (t->color_ != color::black) {
								t->scan_black();
							}
						}
						scan_black(next(first), last);
					}
				}

			public:
				void collect_white() {
					if (color_ == color::white) {
						color_ = color::black;
						collect_white(begin(value_), end(value_));
						delete this;
					}
				}

			private:
				template <typename First, typename Last>
				static
				void collect_white(First const& first, Last const& last) {
					if (not_equal(first, last)) {
						deref(first).collect_white();
						collect_white(next(first), last);
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

			template <typename U, bool Acyclic> friend class detail::counted;
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
