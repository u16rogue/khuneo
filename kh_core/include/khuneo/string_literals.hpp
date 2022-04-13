#pragma once

namespace khuneo
{
	using hash_t = unsigned long;

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
	template <int sz>
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

		consteval string_literal(const char (&_str)[sz])
		{
			for (int i = 0; i < sz; ++i)
			{
				hash = (hash ^ _str[i]) * 0x01000193;
				str[i] = _str[i];
			}
		}

		char      str[sz] { 0 }; // TODO: maybe we shouldn't include the null terminator anymore since its unecessary with what and how we're using this
		const int length  { sz - 1 };
		hash_t    hash    { 0x811c9dc5 }; // fnv1a hashed

		auto match(const char * other) const noexcept -> bool
		{
			for (int i = 0; i < length; ++i)
			{
				if (other[i] != str[i])
					return false;

				if (i == length - 1)
					return other[i + 1] == str[i + 1] == 0;
			}

			return false;
		}

		auto operator==(const char * const rhs) const noexcept -> bool
		{
			return match(rhs);
		}
	};
}