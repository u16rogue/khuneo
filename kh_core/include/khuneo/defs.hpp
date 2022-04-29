#pragma once

namespace khuneo
{
	using kh_bytecode_t    = unsigned char;
	static_assert(sizeof(kh_bytecode_t) == 1, "Expected khuneo::kh_bytecode_t type to have the size of 1 byte");

	using kh_allocator_t   = void*(*)(int sz);
	using kh_deallocator_t = bool(*)(void * p, int sz);
}