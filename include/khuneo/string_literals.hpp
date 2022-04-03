#pragma once

namespace khuneo
{
	template <typename T, int sz>
	struct string_literal
	{
		consteval string_literal(const T (&_str)[sz])
		{
			for (int i = 0; i < sz; ++i)
				str[i] = _str[i];
		}

		T str[sz] { 0 };
		const int length  { sz };
	};
}