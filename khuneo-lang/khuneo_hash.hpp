#pragma once

namespace khuneo
{
	template <typename fnv_t> requires ((sizeof(fnv_t) == 4 || sizeof(fnv_t) == 8))
	constexpr auto hash(const char * str) -> fnv_t
	{
		fnv_t result = sizeof(fnv_t) == 8 ? 0xcbf29ce484222325 : 0x811c9dc5;

		while (*str)
		{
			result = (result * (sizeof(fnv_t) == 8 ? 0x00000100000001B3 : 0x01000193)) ^ *str;
			++str;
		}

		return result;
	}
}