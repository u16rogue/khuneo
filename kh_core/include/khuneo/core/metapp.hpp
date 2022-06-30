#pragma once

namespace khuneo::metapp::details
{
	struct empty_struct {};
	class  empty_class {};
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
}