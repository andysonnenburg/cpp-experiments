#ifndef WART_VARIANT_DETAIL_VARIANT_HPP
#define WART_VARIANT_DETAIL_VARIANT_HPP

#include "enable_if_elem.hpp"
#include "elem_index.hpp"

#include "../../all.hpp"
#include "../../enable_if_move_constructible.hpp"
#include "../../union.hpp"

#include <type_traits>
#include <utility>

namespace wart { namespace detail { namespace variant {

template <typename... T>
using union_t = wart::union_t<T...>;

template <typename... T>
class variant;

class uninitialized {};

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

template <typename... T>
struct copy_assign {
	union_t<uninitialized, T...> union_;
	template <typename U>
	void operator()(U const& value) {
		union_get<U>(union_) = value;
	}
};

template <typename... T>
struct copy_assign_reindex {
	variant<T...>& variant;
	template <typename U>
	void operator()(U const& value) {
		if (variant.tag_ == elem_index<U, T...>::value) {
			union_get<U>(variant.union_) = value;
		} else {
			variant.accept(destroy{});
			new (&variant.union_) U(value);
			variant.tag_ = elem_index<U, T...>::value;
		}
	}
};

template <typename... T>
struct move_assign {
	union_t<uninitialized, T...> union_;
	template <typename U>
	typename enable_if_move_constructible<U>::type operator()(U&& value) {
		union_get<U>(union_) = std::move(value);
	}
};

template <typename... T>
struct move_assign_reindex {
	variant<T...>& variant;
	template <typename U>
	typename enable_if_move_constructible<U>::type operator()(U&& value) {
		if (variant.tag_ == elem_index<U, T...>::value) {
			union_get<U>(variant.union_) = std::move(value);
		} else {
			variant.accept(destroy{});
			new (&variant.union_) U(std::move(value));
			variant.tag_ = elem_index<U, T...>::value;
		}
	}
};

struct trivially_destructible {};

template <typename Variant>
struct not_trivially_destructible {
	~not_trivially_destructible() {
		static_cast<Variant*>(this)->accept(destroy{});
	}
};

template <typename... T>
class variant:
		public
		std::conditional<all<std::is_trivially_destructible<T>::value...>::value,
		                 trivially_destructible,
		                 not_trivially_destructible<variant<T...>>>::type {
	int tag_;
	union_t<uninitialized, T...> union_;

public:
	template <typename F>
	using result_of = common_result_of<F, T...>;
	template <typename F>
	using result_of_t = typename result_of<F>::type;

	template <typename U>
	constexpr
	variant(U const& value,
	        typename enable_if_elem<U, T...>::type* = nullptr):
		tag_{elem_index<U, T...>::value},
		union_{value} {}

	template <typename U>
	constexpr
	variant(U&& value,
	        typename enable_if_elem<U, T...>::type* = nullptr,
	        typename enable_if_move_constructible<U>::type* = nullptr):
		tag_{elem_index<U, T...>::value},
		union_{std::move(value)} {}

	variant(variant const& rhs):
		tag_{rhs.tag_} {
		rhs.accept(copy_construct{&union_});
	}

	template <typename... U>
	variant(variant<U...> const& rhs,
	        typename std::enable_if<all<elem<U, T...>::value...>::value
	        >::type* = nullptr):
		tag_{rhs.accept(copy_construct_index<T...>{&union_})} {}

	variant(variant&& rhs):
		tag_{rhs.tag_} {
		std::move(rhs).accept(move_construct{&union_});
	}

	template <typename... U>
	variant(variant<U...>&& rhs,
	        typename std::enable_if<
	        all<elem<U, T...>::value...>::value
	        >::type* = nullptr):
		tag_{std::move(rhs).accept(move_construct_index<T...>{&union_})} {}

	variant& operator=(variant const& rhs) & {
		static_assert(all<std::is_nothrow_copy_constructible<T>::value...>::value,
		              "all template arguments T must be nothrow copy constructible in class template variant");
		if (this == &rhs) {
			return *this;
		}
		if (tag_ == rhs.tag_) {
			rhs.accept(copy_assign<T...>{union_});
		} else {
			accept(destroy{});
			rhs.accept(copy_construct{&union_});
			tag_ = rhs.tag_;
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
		if (tag_ == rhs.tag_) {
			std::move(rhs).accept(move_assign<T...>{union_});
		} else {
			accept(destroy{});
			std::move(rhs).accept(move_construct{&union_});
			tag_ = rhs.tag_;
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
		using call = result_of_t<F&&> (*)(F&& f, union_t<uninitialized, T...> const&);
		static call calls[] {
			[](F&& f, union_t<uninitialized, T...> const& value) {
				return std::forward<F>(f)(union_get<T>(value));
			}...
		};
		return calls[tag_](std::forward<F>(f), union_);
	}

	template <typename F>
	result_of_t<F> accept(F&& f) & {
		using call = result_of_t<F&&> (*)(F&& f, union_t<uninitialized, T...>&);
		static call calls[] {
			[](F&& f, union_t<uninitialized, T...>& value) {
				return std::forward<F>(f)(union_get<T>(value));
			}...
		};
		return calls[tag_](std::forward<F>(f), union_);
	}

	template <typename F>
	result_of_t<F> accept(F&& f) && {
		using call = result_of_t<F> (*)(F&& f, union_t<uninitialized, T...>&&);
		static call calls[] {
			[](F&& f, union_t<uninitialized, T...>&& value) {
				return std::forward<F>(f)(std::move(union_get<T>(value)));
			}...
		};
		return calls[tag_](std::forward<F>(f), std::move(union_));
	}

	friend
	struct copy_assign_reindex<T...>;

	friend
	struct move_assign_reindex<T...>;
};

}}}

#endif
