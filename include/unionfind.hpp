#ifndef UNIONFIND_HPP
#define UNIONFIND_HPP

#include <memory>
#include <tuple>

namespace unionfind {
	namespace impl {
		template <typename T>
		class repr;

		template <typename T>
		class impl {
		public:
			virtual ~impl() {}
			virtual std::tuple<std::shared_ptr<impl<T> >, repr<T>&> find() = 0;
			virtual std::tuple<std::shared_ptr<impl<T> const> const, repr<T> const&> find() const = 0;
		};

		template <typename T>
		class repr: public impl<T> {
		public:
			int rank;
			T value;
			explicit repr(T const&);
			~repr();
			std::tuple<std::shared_ptr<impl<T> >, repr<T>&> find();
			std::tuple<std::shared_ptr<impl<T> const> const, repr<T> const&> find() const;
		};
		
		template <typename T>
		class link: public impl<T> {
			std::shared_ptr<impl<T> > link_;
		public:
			~link();
			std::tuple<std::shared_ptr<impl<T> >, repr<T>&> find();
			std::tuple<std::shared_ptr<impl<T> const> const, repr<T> const&> find() const;
		};
	}

	template <typename T>
	class ptr {
		impl::link<T> link_;
	public:
		explicit ptr(T const&);
		ptr(ptr<T> const&);
		~ptr();
		ptr<T> const& operator=(ptr<T> const&);
		T& operator*();
		T const& operator*() const;
		T* operator->();
		T const* operator->() const;
	};
}

#endif
