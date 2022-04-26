#pragma once

// Based off https://github.com/Cyan4973/xxHash/issues/496

namespace khuneo::xxh
{
	using xxh32 = unsigned int;
	static_assert(sizeof(xxh32) == 4, "khuneo::xxh::xxh32 is expected to have the size of 32 bits");

	auto hash32(const char * buffer, int length, unsigned int seed = 0) -> xxh32;
}