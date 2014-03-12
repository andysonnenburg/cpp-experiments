#ifndef SHARED_CYCLIC_ITERATION_HPP
#define SHARED_CYCLIC_ITERATION_HPP

namespace shared {
	namespace cyclic {
		namespace result_of {
			template <typename T>
			struct begin {
				typedef extension::begin<T>::type type;
			};
		}

		namespace extension {
			template <typename T>
			struct begin {
				typedef decltype(std::declval<T>().begin()) type;
				static type call(T const& seq) {
					return seq.begin();
				}
			};
		}

		template <typename T>
		typename result_of::begin<T>::type begin(T const& seq) {
			return extension::begin<T>::call(seq);
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
	}
}

#endif
