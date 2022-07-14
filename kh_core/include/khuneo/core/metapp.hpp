#pragma once

namespace khuneo::metapp::details
{
	struct empty_struct {};
	class  empty_class {};

	struct invalid_type { enum { KH_FLAG_IS_INVALID }; };
}

namespace khuneo::metapp
{
	// Allows type switching based off an expression
	template <bool expr, typename T, typename F>
	struct type_if
	{
		/*
		using type = decltype([]() consteval -> auto
		{
			if constexpr (expr)
				return *reinterpret_cast<T *>(0);
			else
				return *reinterpret_cast<F *>(0);
		}());
		*/
	};

	template <typename T, typename F>
	struct type_if<true, T, F> { using type = T; };

	template <typename T, typename F>
	struct type_if<false, T, F> { using type = F; };

	// -------------------------------------------------- 

	// Extends a structure based off an expression which allows to either enable or disable certain parts of a structure
	template <bool expr, typename T>
	struct extend_struct_if : public type_if<expr, T, details::empty_struct>::type {};

	// -------------------------------------------------- 

	// Checks if a type is an invalid khuneo type, this is only valid for
	// types in the khuneo library
	template <typename T>
	struct is_t_invalid { enum { VALUE = requires { T::KH_FLAG_IS_INVALID; } }; };

	// --------------------------------------------------  

	// Obtains the size of an array
	template <typename T, int sz>
	consteval auto array_size(const T (&v)[sz]) -> int
	{
		return sz;
	}

	// --------------------------------------------------  

	template <typename A, typename... B>
	constexpr auto match_any_a_to_b(A a, B... b) -> bool
	{
		return ((a == b) || ...);
	}
}