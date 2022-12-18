#pragma once

#include <khuneo/core/defs.hpp>

namespace khuneo::utf8
{
	constexpr auto c_is_utf8(const char8_t c) -> bool
	{
		return (c & 0b10000000);
	}

	/*
	* Calculates the size of a UTF-8 Character
	*/
	constexpr auto csize(const char8_t b) -> int
	{
		if (!c_is_utf8(b))
			return 1;

		char8_t x = 0b11111000;
		char8_t y = 0b11110000;

		for (int i = 0; i < 3; ++i)
		{
			if ((b & x) == y)
				return 4 - i;
			x <<= 1;
			y <<= 1;
		}

		return 0;
	}

	constexpr auto c_is_numeric(const char8_t c, khuneo::u8 * out = nullptr) -> bool
	{
		if (c >= '0' && c <= '9')
		{
			if (out)
				*out = c - '0';
			return true;
		}

		return false;
	}

	constexpr auto c_is_hex(const char8_t c, khuneo::u8 * out = nullptr) -> bool
	{
		if (c >= 'A' && c <= 'F')
		{
			if (out)
				*out = c - 'A' + 0xA;
			return true;
		}
		else if (c >= 'a' && c <= 'f')
		{
			if (out)
				*out = c - 'a' + 0xa;
			return true;
		}
		else if (c_is_numeric(c, out))
		{
			return true;
		}

		return false;
	}

	constexpr auto c_is_alpha(const char8_t c) -> bool
	{
		return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
	}

	constexpr auto c_is_alphanumeric(const char8_t c) -> bool
	{
		return c_is_alpha(c) || c_is_numeric(c);
	}

	/*
	* Calculates the length of a UTF-8 string, if the parameter e is not
	* provided it would instead check for null terminator
	*/
	constexpr auto slength(const char8_t * s, const char8_t * e = nullptr) -> int
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
	constexpr auto ssize(const char8_t * s, const char8_t * e = nullptr) -> int
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