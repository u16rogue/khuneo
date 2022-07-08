#pragma once

namespace khuneo::utf8
{
	/*
	* Calculates the size of a UTF-8 Character
	*/
	constexpr auto csize(const char b) -> int
	{
		if ((b & 0b10000000) == 0b00000000)
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
	* Calculates the length of a UTF-8 string, if the parameter e is not
	* provided it would instead check for null terminator
	*/
	constexpr auto slength(const char * s, const char * e = nullptr) -> int
	{
		int len = 0;
		while ((e && s < e) || (!e && *s))
		{
			++len;
			s += csize(*s);
		}

		return len;
	}

	/*
	* Calculates the size of a UTF-8 string, if the parameter e is not
	* provided it would instead check for a null terminator
	*/
	constexpr auto ssize(const char * s, const char * e = nullptr) -> int
	{
		int size = 0;
		while ((e && s < e) || (!e && *s))
		{
			int cvl = csize(*s);
			size += cvl;
			s += cvl;
		}

		return size;
	}
}