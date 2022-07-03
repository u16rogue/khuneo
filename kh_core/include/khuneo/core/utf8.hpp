#pragma once

namespace khuneo::utf8
{
	/*
	* Calculates the size of a UTF-8 Character
	*/
	constexpr auto size(const char b) -> int
	{
		if ((b & 0b11000000) == 0b00000000)
			return 1;

		char x = 0b11111000;
		char y = 0b11110000;

		for (int i = 0; i < 3; ++i)
		{
			if ((b & x) == y)
				return 4 - i;
			x <<= 1;
			y <<= 1;
		}

		return 0;
	}

	/*
	* Calculates the size of a UTF-8 Character
	*/
	constexpr auto size(const char * s) -> int
	{
		return size(*s);
	}

	/*
	* Calculates the length of a UTF-8 string, if the parameter e is not
	* provided it would instead check for null termination
	*/
	constexpr auto length(const char * s, const char * e = nullptr) -> int
	{
		int len = 0;
		while ((e && s < e) || (!e && *s))
		{
			int cvl = size(s);
			len += cvl;
			s   += cvl;
		}

		return len;
	}
}