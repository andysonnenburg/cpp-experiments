#ifndef UNIONFIND_HPP
#define UNIONFIND_HPP

#include <memory>
#include <tuple>

namespace unionfind {
	namespace detail {
    using namespace std;

		template <typename T>
		class set {
		public:
			virtual ~set() {}
			virtual tuple<shared_ptr<set<T>>&, int&, unique_ptr<T>&> find(shared_ptr<set<T>> const&) = 0;
		};

		template <typename T>
		class repr: public set<T> {
			int rank_;
			unique_ptr<T> ptr_;
		public:
			explicit repr(T*);
			~repr();
			tuple<shared_ptr<set<T>>&, int&, unique_ptr<T>&> find(shared_ptr<set<T>> const&);
		};
		
		template <typename T>
		class link: public set<T> {
			shared_ptr<set<T>> link_;
		public:
      link(shared_ptr<set<T>> const&);
			~link();
			tuple<shared_ptr<set<T>>&, int&, unique_ptr<T>&> find(shared_ptr<set<T>> const&);
		};
	}

	template <typename T>
	class ptr {
		std::shared_ptr<detail::set<T>> ptr_;
	public:
		explicit ptr(T*);
		ptr(ptr<T> const&);
		~ptr();
		ptr<T>& operator=(ptr<T> const&);
		T& operator*();
		T const& operator*() const;
		T* operator->();
		T const* operator->() const;
	};
}


template <typename T>
inline
unionfind::ptr<T>::ptr(T* value):
	ptr_(std::shared_ptr<detail::set<T>>(new detail::repr<T>(value))) {}

template <typename T>
inline
unionfind::ptr<T>::~ptr() {}

template <typename T>
unionfind::ptr<T> const& unionfind::ptr<T>::operator=(ptr<T> const& rhs) {
	using namespace std;
	auto tuple_x = ptr_->find(ptr_);
	auto& ptr_x = get<0>(tuple_x);
  auto tuple_y = rhs.ptr_->find(rhs.ptr_);
	auto& ptr_y = get<0>(tuple_y);
	if (ptr_x == ptr_y) {
		return *this;
	}
	auto& rank_x = get<1>(tuple_x);
	auto& rank_y = get<1>(tuple_y);
	if (rank_x < rank_y) {
		ptr_x = ptr_y;
	} else if (rank_x == rank_y) {
		++rank_y;
		ptr_x = ptr_y;
	} else {
		get<2>(tuple_x) = repr_y.value;
		*ptr_y = detail::link<T>(ptr_x);
	}
	return *this;
}

#endif
