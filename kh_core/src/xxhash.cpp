#include <khuneo/xxhash.hpp>

static auto khuneo_xxh_rotl32(khuneo::xxh::xxh32 v, int shift) -> khuneo::xxh::xxh32
{
	if (shift == 0 || shift == 32)
		return v;

	if (shift > 32)
		shift %= 32;

	return (v << shift) | v >> (32 - shift);
}

auto khuneo::xxh::hash32(const char * buffer, int length, unsigned int seed) -> xxh32
{
	constexpr xxh32 p[] = {	0x9E3779B1U, 0x85EBCA77U, 0xC2B2AE3DU, 0x27D4EB2FU, 0x165667B1U };
	xxh32 v[] = { seed + p[0] + p[1], seed + p[1], seed, seed - p[0] };

	int pos = 0;
	// If the data is larger than 16 process each 16 bytes of blocks until we get the final block
	for (; pos + 16 <= length; pos += 16)
		for (auto & _v : v)
			_v = khuneo_xxh_rotl32(_v + *reinterpret_cast<const xxh32 *>(buffer + pos + ((&_v - v) * 4)) * p[1], 13) * p[0];

	xxh32 h = length;
	if (length >= 16)
	{
		constexpr int s[] = { 1, 7, 12, 18 };
		for (int i = 0; i < 4; ++i)
			h += khuneo_xxh_rotl32(v[i], s[i]);
	}
	else
	{
		h += (v[2] + p[4]);
	}

	for (; length - pos >= 4; pos += 4)
		h = khuneo_xxh_rotl32((h + *reinterpret_cast<const xxh32 *>(buffer + pos) * p[2]), 17) * p[3];

	for (; length - pos > 0; ++pos)
		h = khuneo_xxh_rotl32((h + *reinterpret_cast<const unsigned char *>(buffer + pos) * p[4]), 11) * p[0];

	h ^= h >> 15;
	h *= p[1];
	h ^= h >> 13;
	h *= p[2];
	h ^= h >> 16;

	return h;
}