#ifndef SHARED_CYCLIC_HPP
#define SHARED_CYCLIC_HPP

namespace shared {
	namespace cyclic {
		namespace detail {
			enum class color {
				Black,
				Gray,
				White
			};

			template <typename T, typename Count>
			class counted {
				T value_;
				Count count_;

			public:
				template <typename... Args>
				counted(Args&&... args):
					value_(std::forward<Args>(args)...),
					count_(1) {}

				T& value() {
					return value_;
				}
			};
		}

		template <typename T, typename Count = unsigned int>
		class ptr {
			detail::counted<T, Count>* counted_;
			T* ptr_;

		public:
			template <typename... Args>
			explicit ptr(Args&&... args):
				counted_(new detail::counted<T, Count>(std::forward<Args>(args)...)),
				ptr_(&counted_->value()) {}
		};
	}
}

#endif
