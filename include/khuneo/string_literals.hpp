#pragma once

namespace khuneo
{
	template <typename T, int sz>
	struct string_literal
	{
		using type = T;

		consteval string_literal(const T (&_str)[sz])
		{
			for (int i = 0; i < sz; ++i)
				str[i] = _str[i];
		}

		T str[sz] { 0 };
		const int length  { sz - 1 };

		auto match(const T * other) -> bool
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

		auto operator==(const T * rhs) -> bool
		{
			return match(rhs);
		}
	};

	// For single characters
	template <typename T>
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
}