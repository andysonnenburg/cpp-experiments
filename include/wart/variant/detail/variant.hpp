#ifndef WART_VARIANT_DETAIL_VARIANT_HPP
#define WART_VARIANT_DETAIL_VARIANT_HPP

#include "all.hpp"
#include "enable_if_move_constructible.hpp"
#include "enable_if_elem.hpp"
#include "elem_index.hpp"
#include "union_storage.hpp"

#include <type_traits>
#include <utility>

namespace wart { namespace detail { namespace variant {

template <typename... T>
class variant;

template <typename F, typename... ArgTypes>
using common_result_of =
	std::common_type<typename std::result_of<F(ArgTypes)>::type...>;

struct destroy {
	template <typename T>
	void operator()(T&& value) {
		using type = typename std::remove_reference<T>::type;
		std::forward<T>(value).~type();
	}
};

struct copy_construct {
	void* storage;
	template <typename T>
	void operator()(T const& value) {
		new (storage) T(value);
	}
};

template <typename... T>
struct copy_construct_index {
	void* storage;
	template <typename U>
	int operator()(U const& value) {
		new (storage) U(value);
		return elem_index<U, T...>::value;
	}
};

struct move_construct {
	void* storage;
	template <typename T>
	typename enable_if_move_constructible<T>::type operator()(T&& value) {
		new (storage) T(std::move(value));
	}
};

template <typename... T>
struct move_construct_index {
	void* storage;
	template <typename U>
	typename enable_if_move_constructible<U, int>::type operator()(U&& value) {
		new (storage) U(std::move(value));
		return elem_index<U, T...>::value;
	}
};

struct copy_assign {
	void* storage;
	template <typename T>
	void operator()(T const& value) {
		*static_cast<T*>(storage) = value;
	}
};

template <typename... T>
struct copy_assign_reindex {
	variant<T...>& variant;
	template <typename U>
	void operator()(U const& value) {
		if (variant.which_ == elem_index<U, T...>::value) {
			*static_cast<U*>(static_cast<void*>(&variant.storage_)) = value;
		} else {
			variant.accept(destroy{});
			new (&variant.storage_) U(value);
			variant.which_ = elem_index<U, T...>::value;
		}
	}
};

struct move_assign {
	void* storage;
	template <typename T>
	typename enable_if_move_constructible<T>::type operator()(T&& value) {
		*static_cast<T*>(storage) = std::move(value);
	}
};

template <typename... T>
struct move_assign_reindex {
	variant<T...>& variant;
	template <typename U>
	typename enable_if_move_constructible<U>::type operator()(U&& value) {
		if (variant.which_ == elem_index<U, T...>::value) {
			*static_cast<U*>(static_cast<void*>(&variant.storage_)) = std::move(value);
		} else {
			variant.accept(destroy{});
			new (&variant.storage_) U(std::move(value));
			variant.which_ = elem_index<U, T...>::value;
		}
	}
};

template <typename... T>
class variant {
	int which_;
	union_storage_t<T...> storage_;

public:
	template <typename F>
	using result_of = common_result_of<F, T...>;
	template <typename F>
	using result_of_t = typename result_of<F>::type;

	template <typename U>
	variant(U const& value,
	        typename enable_if_elem<U, T...>::type* = nullptr):
		which_{elem_index<U, T...>::value} {
		new (&storage_) U(value);
	}

	template <typename U>
	variant(U&& value,
	        typename enable_if_elem<U, T...>::type* = nullptr,
	        typename enable_if_move_constructible<U>::type* = nullptr):
		which_{elem_index<U, T...>::value} {
		new (&storage_) U(std::move(value));
	}

	variant(variant const& rhs):
		which_{rhs.which_} {
		rhs.accept(copy_construct{&storage_});
	}

	template <typename... U>
	variant(variant<U...> const& rhs,
	        typename std::enable_if<all<elem<U, T...>::value...>::value
	        >::type* = nullptr):
		which_{rhs.accept(copy_construct_index<T...>{&storage_})} {}

	variant(variant&& rhs):
		which_{rhs.which_} {
		std::move(rhs).accept(move_construct{&storage_});
	}

	template <typename... U>
	variant(variant<U...>&& rhs,
	        typename std::enable_if<
	        all<elem<U, T...>::value...>::value
	        >::type* = nullptr):
		which_{std::move(rhs).accept(move_construct_index<T...>{&storage_})} {}

	~variant() {
		accept(destroy{});
	}

	variant& operator=(variant const& rhs) & {
		static_assert(all<std::is_nothrow_copy_constructible<T>::value...>::value,
		              "all template arguments T must be nothrow copy constructible in class template variant");
		if (this == &rhs) {
			return *this;
		}
		if (which_ == rhs.which_) {
			rhs.accept(copy_assign{&storage_});
		} else {
			accept(destroy{});
			rhs.accept(copy_construct{&storage_});
			which_ = rhs.which_;
		}
		return *this;
	}

	template <typename... U>
	variant& operator=(variant<U...> const& rhs) & {
		static_assert(all<std::is_nothrow_copy_constructible<T>::value...>::value,
		              "all template arguments T must be nothrow copy constructible in class template variant");
		rhs.accept(copy_assign_reindex<T...>{*this});
		return *this;
	}

	variant& operator=(variant&& rhs) & {
		static_assert(all<std::is_nothrow_move_constructible<T>::value...>::value,
		              "all template arguments T must be nothrow move constructible in class template variant");
		if (this == &rhs) {
			return *this;
		}
		if (which_ == rhs.which_) {
			std::move(rhs).accept(move_assign{&storage_});
		} else {
			accept(destroy{});
			std::move(rhs).accept(move_construct{&storage_});
			which_ = rhs.which_;
		}
		return *this;
	}

	template <typename... U>
	variant& operator=(variant<U...>&& rhs) & {
		static_assert(all<std::is_nothrow_copy_constructible<T>::value...>::value,
		              "all template arguments T must be nothrow copy constructible in class template variant");
		std::move(rhs).accept(move_assign_reindex<T...>{*this});
		return *this;
	}

	template <typename F>
	result_of_t<F> accept(F&& f) const& {
		using call = result_of_t<F&&> (*)(F&& f, union_storage_t<T...> const&);
		static call calls[] {
			[](F&& f, union_storage_t<T...> const& value) {
				return std::forward<F>(f)(*static_cast<T const*>(static_cast<void const*>(&value)));
			}...
		};
		return calls[which_](std::forward<F>(f), storage_);
	}

	template <typename F>
	result_of_t<F> accept(F&& f) & {
		using call = result_of_t<F&&> (*)(F&& f, union_storage_t<T...>&);
		static call calls[] {
			[](F&& f, union_storage_t<T...>& value) {
				return std::forward<F>(f)(*static_cast<T*>(static_cast<void*>(&value)));
			}...
		};
		return calls[which_](std::forward<F>(f), storage_);
	}

	template <typename F>
	result_of_t<F> accept(F&& f) && {
		using call = result_of_t<F> (*)(F&& f, union_storage_t<T...>&&);
		static call calls[] {
			[](F&& f, union_storage_t<T...>&& value) {
				return std::forward<F>(f)(std::move(*static_cast<T*>(static_cast<void*>(&value))));
			}...
		};
		return calls[which_](std::forward<F>(f), std::move(storage_));
	}

	friend
	struct copy_assign_reindex<T...>;

	friend
	struct move_assign_reindex<T...>;
};

}}}

#endif
