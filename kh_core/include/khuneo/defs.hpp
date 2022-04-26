#pragma once

namespace khuneo
{
	using kh_allocator_t   = void*(*)(int sz);
	using kh_deallocator_t = bool(*)(void * p, int sz);
}