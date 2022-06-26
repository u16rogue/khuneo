#pragma once

// Based off https://github.com/Cyan4973/xxHash/issues/496

#include <kh_core/defs.hpp>

namespace khuneo::hash::details
{
	namespace xxh32
	{
		constexpr khuneo::u32 p[] = { 0x9E3779B1U, 0x85EBCA77U, 0xC2B2AE3DU, 0x27D4EB2FU, 0x165667B1U }; // Primes
		constexpr int s[]         = { 1, 7, 12, 18 };
	}
}

namespace khuneo::hash
{
	struct xxh32_t { khuneo::u32 v; };

	constexpr auto xxh32(const char * buffer, const int size = 0, khuneo::u32 seed = 0) -> xxh32_t
	{
		constexpr auto rotl = [](khuneo::u32 v, int shift) constexpr -> khuneo::u32
		{
			if (shift == 0 || shift == 32)
				return v;

			if (shift > 32)
				shift %= 32;

			return (v << shift) | v >> (32 - shift);
		};

		constexpr auto as_u32 = [](const char * b/*uffer*/, int i/*ndex*/) constexpr -> khuneo::u32
		{
			return khuneo::u32(b[i]) | khuneo::u32(b[i + 1]) << 8 | khuneo::u32(b[i + 2]) << 16 | khuneo::u32(b[i + 3]) << 24;
		};

		if (!buffer || !size)
			return { 0 };

		using namespace details::xxh32;
		khuneo::u32 v[] = { seed + p[0] + p[1], seed + p[1], seed, seed - p[0] }; // Value

		// Consume chunks by 16 bytes until we're left with the final block of maximum 16 bytes
		int i = 0; // buffer index

		for (; i < size - 16; i += 16)
		{
			for (int vi = 0; vi < sizeof(v) / sizeof(v[0]); ++vi)
			{
				auto & _v = v[vi];
				_v = rotl(_v + as_u32(buffer, i + (vi * 4)) * p[1], 13) * p[0];
			}
		}

		khuneo::u32 h = size;
		if (size >= 16)
			for (int si = 0; si < sizeof(s) / sizeof(s[0]); ++si)
				h += rotl(v[si], s[si]);
		else
			h += v[2] + p[4];

		// Consume chunks of strictly only 4 bytes
		for (; i <= size - 4; i += 4)
			h = rotl((h + as_u32(buffer, i) * p[2]), 17) * p[3];

		// Consume the rest of the bytes
		for (; i < size; ++i)
			h = rotl((h + buffer[i] * p[4]), 11) * p[0];

		h ^= h >> 15;
		h *= p[1];
		h ^= h >> 13;
		h *= p[2];
		h ^= h >> 16;

		return { h };
	}

	constexpr auto xxh32_str(const char * str) -> xxh32_t
	{
		int len = 0;
		while (str[len]) ++len; // strlen
		return xxh32(str, len);
	}

	template <int sz>
	consteval auto xxh32_str_cv(const char (&str)[sz]) -> xxh32_t
	{
		return xxh32(str, sz - 1);
	}

	
}
