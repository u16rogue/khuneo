#pragma once

namespace khuneo
{
	/*
	* Represents a string that is enforced as compile time
	* string literal, constructor is marked as consteval to
	* enforce this which also allows us to use it as a template
	* parameter.
	* 
	* T  = string type
	* sz = Size of string buffer (including the null terminator)
	* 
	* These template parameters can be automatically be inferred by
	* simply constructing an instance of it
	*/
	template <typename T, int sz>
	struct string_literal
	{
		/*
		* Already tested this implementation on clang 14.0 with -O3
		* and the resulting machine code seems to be very efficient,
		* it compares multiple bytes at the sametime versus the traditional
		* string compare, given that the entire string is known at compile time
		* and is enforced, debug builds are really horrible though, each instance
		* of a string_literal<> gets its own copy (obv) which makes it undesirable
		* on debug mode.
		* 
		* This note is here incase a better solution that actually works with better
		* insight.
		* 
		* Debating whether the operator== should even exist, it doesn't feel like it fits
		* here and serves no advantage other than an abstraction to string_literal::match
		*/

		using type = T;

		consteval string_literal(const T (&_str)[sz])
		{
			for (int i = 0; i < sz; ++i)
				str[i] = _str[i];
		}

		T str[sz] { 0 }; // TODO: maybe we shouldn't include the null terminator anymore since its unecessary with what and how we're using this
		const int length  { sz - 1 };

		auto match(const T * other) const noexcept -> bool
		{
			for (int i = 0; i < length; ++i)
			{
				if (other[i] != str[i])
					return false;

				if (i == length - 1)
					return true;
			}

			return false;
		}

		auto operator==(const T * const rhs) const noexcept -> bool
		{
			return match(rhs);
		}
	};

	#if 0
	// For single characters
	template <typename T, 1>
	struct string_literal
	{
		using type = T;

		consteval string_literal(const T &_str)
			: str(_str)
		{
		}

		T str { 0 };
		const int length { 1 };

		auto match(const T * other) -> bool
		{
			return str == *other;
		}

		auto operator==(const T * rhs) -> bool
		{
			return match(rhs);
		}
	};
	#endif
}