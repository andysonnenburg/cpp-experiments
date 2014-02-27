#ifndef WART_VARIANT_HPP
#define WART_VARIANT_HPP

#include <type_traits>
#include <utility>

#include "union_storage.hpp"

namespace wart {
	namespace detail {
		namespace variant {
			template <typename Elem, typename Head, typename... Tail>
			struct elem_index;

			template <typename Head, typename... Tail>
			struct elem_index<Head, Head, Tail...>:
				std::integral_constant<int, 0> {};

			template <typename Elem, typename Head, typename... Tail>
			struct elem_index:
				std::integral_constant<int, elem_index<Elem, Tail...>::value + 1> {};

			template <bool... List>
			struct all;

			template <>
			struct all<>: std::true_type {};

			template <bool... Tail>
			struct all<true, Tail...>: all<Tail...>::type {};

			template <bool... Tail>
			struct all<false, Tail...>: std::false_type {};

			struct copier {
				void* memory;
				template <typename T>
				void operator()(T const& value) {
					new (memory) T(value);
				}
			};

			struct assigner {
				void* memory;
				template <typename T>
				void operator()(T const& value) {
					*static_cast<T*>(memory) = value;
				}
			};

			struct destroyer {
				template <typename T>
				void operator()(T const& value) {
					value.~T();
				}
			};

			template <typename Variant>
			struct unassignable {};

			template <typename Variant>
			struct assignable {
				Variant& assign(Variant const& rhs) & {
					if (self() == &rhs) {
						return *self();
					}
					if (self()->which_ == rhs.which_) {
						rhs.accept(assigner{&self()->union_storage_});
					} else {
						self()->accept(destroyer{});
						self()->which_ = rhs.which_;
						rhs.accept(copier{&self()->union_storage_});
					}
					return *self();
				}

			private:
				Variant* self() {
					return static_cast<Variant*>(this);
				}

				Variant const* self() const {
					return static_cast<Variant*>(this);
				}
			};
		}
	}

	template <typename... Types>
	class variant:
		std::conditional<
		detail::variant::all<std::is_nothrow_constructible<Types>::value...>::value,
		detail::variant::assignable<variant<Types...>>,
		detail::variant::unassignable<variant<Types...>>>::type {
		int which_;
		union_storage<Types...> union_storage_;

	public:
		template <typename T>
		variant(T const& value): which_{detail::variant::elem_index<T, Types...>::value} {
			new (&union_storage_) T{value};
		}

		variant(variant const& rhs): which_{rhs.which_} {
			rhs.accept(detail::variant::copier{&union_storage_});
		}

		~variant() {
			accept(detail::variant::destroyer{});
		}

		variant& operator=(variant const& rhs) & {
			static_assert
				(detail::variant::all<std::is_nothrow_constructible<Types>::value...>::value,
				 "variant is assignable only when the contained types are nothrow constructible");
			return detail::variant::assignable<variant>::assign(rhs);
		}

		template <typename F>
		typename std::common_type<
			typename std::result_of<F(Types)>::type...
			>::type accept(F&& f) const {
			using result_type = typename std::common_type<
				typename std::result_of<F(Types)>::type...
				>::type;
			using call = result_type (*)(F&& f, union_storage<Types...> const&);
			static call calls[] {
				[](F&& f, union_storage<Types...> const& value) {
					return std::forward<F>(f)(union_cast<Types, Types...>(value));
				}...
					};
			return calls[which_](std::forward<F>(f), union_storage_);
		}

		template <typename F>
		typename std::common_type<
			typename std::result_of<F(Types)>::type...
			>::type accept(F&& f) {
			using result_type = typename std::common_type<
				typename std::result_of<F(Types)>::type...
				>::type;
			using call = result_type (*)(F&& f, union_storage<Types...>&);
			static call calls[] {
				[](F&& f, union_storage<Types...>& value) {
					return std::forward<F>(f)(union_cast<Types, Types...>(value));
				}...
					};
			return calls[which_](std::forward<F>(f), union_storage_);
		}

		friend struct detail::variant::assignable<variant<Types...>>;
	};
}

#endif
