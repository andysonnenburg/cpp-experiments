#ifndef UNIONFIND_HPP
#define UNIONFIND_HPP

#include <memory>
#include <tuple>

namespace unionfind {
	namespace impl {
    using namespace std;

    class uncopyable {
    protected:
      uncopyable() = default;
      ~uncopyable() = default;
      uncopyable(uncopyable const&) = delete;
      uncopyable(uncopyable&&) = delete;
      uncopyable& operator=(uncopyable const &) = delete;
      uncopyable& operator=(uncopyable&&) = delete;
    };

		template <typename T>
		class repr;

		template <typename T>
		class impl: uncopyable {
		public:
			virtual ~impl() {}
			virtual tuple<shared_ptr<impl<T>>&, repr<T>&> find() = 0;
			virtual tuple<shared_ptr<impl<T> const> const&, repr<T> const&> find() const = 0;
		};

		template <typename T>
		class repr: public impl<T> {
		public:
			int rank;
			T value;
			explicit repr(T const&);
			~repr();
			tuple<shared_ptr<impl<T>>&, repr<T>&> find();
			tuple<shared_ptr<impl<T> const> const&, repr<T> const&> find() const;
		};
		
		template <typename T>
		class link: public impl<T> {
			shared_ptr<impl<T>> link_;
		public:
      link(shared_ptr<impl<T>>);
			~link();
      tuple<shared_ptr<impl<T>>&, repr<T>&> find();
			tuple<shared_ptr<impl<T> const> const&, repr<T> const&> find() const;
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
