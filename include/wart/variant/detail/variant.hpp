#ifndef WART_VARIANT_DETAIL_VARIANT_HPP
#define WART_VARIANT_DETAIL_VARIANT_HPP

#include "enable_if_elem.hpp"
#include "elem_index.hpp"
#include "head.hpp"

#include "../../all.hpp"
#include "../../enable_if_move_constructible.hpp"
#include "../../make_tag.hpp"
#include "../../undecayed_common_type.hpp"
#include "../../union.hpp"

#include <type_traits>
#include <utility>

namespace wart { namespace detail { namespace variant {

using wart::union_t;

template <typename... T>
class variant;

class uninitialized {};

template <typename F, typename... ArgTypes>
using common_result_of =
	undecayed_common_type<typename std::result_of<F(ArgTypes)>::type...>;

template <typename F, typename... ArgTypes>
using common_result_of_t = typename common_result_of<F, ArgTypes...>::type;

template <typename U, typename F, typename... T>
inline
common_result_of_t<F&&, T&...>
union_cast_and_call(F&& f, union_t<uninitialized, T...>& value) {
	return std::forward<F>(f)(union_cast<U>(value));
}

template <typename U, typename F, typename... T>
inline
common_result_of_t<F&&, T const&...>
union_cast_and_call(F&& f, union_t<uninitialized, T...> const& value) {
	return std::forward<F>(f)(union_cast<U>(value));
}

template <typename U, typename F, typename... T>
inline
common_result_of_t<F&&, T&&...>
union_cast_and_call(F&& f, union_t<uninitialized, T...>&& value) {
	return std::forward<F>(f)(union_cast<U>(std::move(value)));
}

struct destroy {
	template <typename T>
	void operator()(T&& value) const {
		using type = typename std::remove_reference<T>::type;
		std::forward<T>(value).~type();
	}
};

template <typename... T>
struct copy_construct {
	union_t<uninitialized, T...>& union_;
	template <typename U>
	void operator()(U const& value) {
		new (&union_cast<U>(union_)) U(value);
	}
};

template <typename... T>
struct copy_construct_and_tag {
	union_t<uninitialized, T...>& union_;
	template <typename U>
	int operator()(U const& value) {
		new (&union_cast<U>(union_)) U(value);
		return elem_index<U, T...>::value;
	}
};

template <typename... T>
struct move_construct {
	union_t<uninitialized, T...>& union_;
	template <typename U>
	typename enable_if_move_constructible<U>::type operator()(U&& value) {
		new (&union_cast<U>(union_)) U(std::move(value));
	}
};

template <typename... T>
struct move_construct_and_tag {
	union_t<uninitialized, T...>& union_;
	template <typename U>
	typename enable_if_move_constructible<U, int>::type operator()(U&& value) {
		new (&union_cast<U>(union_)) U(std::move(value));
		return elem_index<U, T...>::value;
	}
};

template <typename... T>
struct copy_assign {
	union_t<uninitialized, T...> union_;
	template <typename U>
	void operator()(U const& value) {
		union_cast<U>(union_) = value;
	}
};

template <typename... T>
struct copy_assign_and_retag {
	variant<T...>* this_;
	template <typename U>
	void operator()(U const& value) {
		if (this_->tag_ == elem_index<U, T...>::value) {
			union_cast<U>(this_->union_) = value;
		} else {
			this_->accept(destroy{});
			new (&this_->union_) U(value);
			this_->tag_ = elem_index<U, T...>::value;
		}
	}
};

template <typename... T>
struct move_assign {
	union_t<uninitialized, T...> union_;
	template <typename U>
	typename enable_if_move_constructible<U>::type operator()(U&& value) {
		union_cast<U>(union_) = std::move(value);
	}
};

template <typename... T>
struct move_assign_and_retag {
	variant<T...>* this_;
	template <typename U>
	typename enable_if_move_constructible<U>::type operator()(U&& value) {
		if (this_->tag_ == elem_index<U, T...>::value) {
			union_cast<U>(this_->union_) = std::move(value);
		} else {
			this_->accept(destroy{});
			new (&this_->union_) U(std::move(value));
			this_->tag_ = elem_index<U, T...>::value;
		}
	}
};

template <bool TriviallyDestructible, typename... T>
struct trivially_destructible;

template <typename... T>
struct trivially_destructible<true, T...> {};

template <typename... T>
struct trivially_destructible<false, T...> {
	~trivially_destructible() {
		static_cast<variant<T...>*>(this)->accept(destroy{});
	}
};

template <typename... T>
using destructible =
	trivially_destructible<
	all<std::is_trivially_destructible<T>::value...>::value,
	T...>;

template <typename... T>
class variant: destructible<T...> {
	int tag_;
	union_t<uninitialized, T...> union_;

public:
	constexpr
	variant():
		tag_{0},
		union_{make_tag<typename head<T...>::type>()} {}

	template <typename U>
	constexpr
	variant(U const& value,
	        typename enable_if_elem<U, T...>::type* = nullptr):
		tag_{elem_index<U, T...>::value},
		union_{make_tag<U>(), value} {}

	template <typename U>
	constexpr
	variant(U&& value,
	        typename enable_if_elem<U, T...>::type* = nullptr,
	        typename enable_if_move_constructible<U>::type* = nullptr):
		tag_{elem_index<U, T...>::value},
		union_{make_tag<U>(), std::move(value)} {}

	variant(variant const& rhs):
		tag_{rhs.tag_} {
		rhs.accept(copy_construct<T...>{union_});
	}

	template <typename... U>
	variant(variant<U...> const& rhs,
	        typename std::enable_if<all<elem<U, T...>::value...>::value
	        >::type* = nullptr):
		tag_{rhs.accept(copy_construct_and_tag<T...>{union_})} {}

	variant(variant&& rhs):
		tag_{rhs.tag_} {
		std::move(rhs).accept(move_construct<T...>{union_});
	}

	template <typename... U>
	variant(variant<U...>&& rhs,
	        typename std::enable_if<
	        all<elem<U, T...>::value...>::value
	        >::type* = nullptr):
		tag_{std::move(rhs).accept(move_construct_and_tag<T...>{union_})} {}

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
			rhs.accept(copy_construct<T...>{union_});
			tag_ = rhs.tag_;
		}
		return *this;
	}

	template <typename... U>
	variant& operator=(variant<U...> const& rhs) & {
		static_assert(all<std::is_nothrow_copy_constructible<T>::value...>::value,
		              "all template arguments T must be nothrow copy constructible in class template variant");
		rhs.accept(copy_assign_and_retag<T...>{this});
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
			std::move(rhs).accept(move_construct<T...>{union_});
			tag_ = rhs.tag_;
		}
		return *this;
	}

	template <typename... U>
	variant& operator=(variant<U...>&& rhs) & {
		static_assert(all<std::is_nothrow_copy_constructible<T>::value...>::value,
		              "all template arguments T must be nothrow copy constructible in class template variant");
		std::move(rhs).accept(move_assign_and_retag<T...>{this});
		return *this;
	}

	template <typename F>
	common_result_of_t<F, T&...> accept(F&& f) & {
		using result_type = common_result_of_t<F&&, T&...>;
		using call = result_type (*)(F&& f, union_t<uninitialized, T...>&);
		static call calls[] {
			union_cast_and_call<T, F, T...>...
		};
		return calls[tag_](std::forward<F>(f), union_);
	}

	template <typename F>
	common_result_of_t<F, T const&...> accept(F&& f) const& {
		using result_type = common_result_of_t<F&&, T const&...>;
		using call = result_type (*)(F&& f, union_t<uninitialized, T...> const&);
		static call calls[] {
			union_cast_and_call<T, F, T...>...
		};
		return calls[tag_](std::forward<F>(f), union_);
	}

	template <typename F>
	common_result_of_t<F, T&&...> accept(F&& f) && {
		using result_type = common_result_of_t<F&&, T&&...>;
		using call = result_type (*)(F&& f, union_t<uninitialized, T...>&&);
		static call calls[] {
			union_cast_and_call<T, F, T...>...
		};
		return calls[tag_](std::forward<F>(f), std::move(union_));
	}

	friend
	struct copy_assign_and_retag<T...>;

	friend
	struct move_assign_and_retag<T...>;

	friend
	destructible<T...>;
};

}}}

#endif
