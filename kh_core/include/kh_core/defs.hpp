#pragma once

#define KH_DEFPRIMT(bitsz, signed_t, unsigned_t) \
	using i##bitsz = signed_t; \
	using u##bitsz = unsigned_t; \
	static_assert(sizeof(u##bitsz) == bitsz / 8 && sizeof(i##bitsz) == bitsz / 8, "[khuneo] type did not match the expected size.");


namespace khuneo
{
	KH_DEFPRIMT(8, char, unsigned char)
	KH_DEFPRIMT(16, short, unsigned short)
	KH_DEFPRIMT(32, int, unsigned int)
	KH_DEFPRIMT(64, long long, unsigned long long)
}

#undef KH_DEFPRIMT 
