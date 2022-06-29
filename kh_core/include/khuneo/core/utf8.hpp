#pragma once

namespace khuneo::utf8
{
	constexpr auto size(const char * s) -> int
	{
		char b = *s;

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
}