#pragma once

namespace khuneo
{
	#define KH_DEFPRIMT(bitsz, signed_t, unsigned_t) \
		using i##bitsz = signed_t; \
		using u##bitsz = unsigned_t; \
		static_assert(sizeof(u##bitsz) == bitsz / 8 && sizeof(i##bitsz) == bitsz / 8, "[khuneo] type did not match the expected size.");

	KH_DEFPRIMT(8, char, unsigned char)
	KH_DEFPRIMT(16, short, unsigned short)
	KH_DEFPRIMT(32, int, unsigned int)
	KH_DEFPRIMT(64, long long, unsigned long long)
	#undef KH_DEFPRIMT 

	
}

namespace khuneo::details
{
	/*
	* kh_allocator - Allocation scheme implementation used by khuneo.
	*
	* -- Implementing your own:
	*		You can implement your own allocation method by creating your
	*		own class and inheriting kh_allocator in a CRTP pattern ( class myalloc : kh_allocator<myalloc> )
	*		and providing a _alloc and _dealloc implementation. The prototype for the following:
	*			alloc   - static void * _alloc(int size);
	*			dealloc - static bool _dealloc(void * p, int size); 
	*/
	template <typename impl = void>
	struct kh_allocator
	{
		static_assert(
			sizeof(impl) == 0
		||  requires { impl::_alloc(0); }
		,   "kh_allocator's implementation must provide a [static void * _alloc(int size)] implementation");

		static_assert(
			sizeof(impl) == 0
		||  requires { impl::_dealloc(nullptr, 0); }
		,   "kh_allocator's implementation must provide a [static bool _dealloc(void * p, int size)] implementation");

		static_assert(
			sizeof(impl) == 0
		||  !(requires { impl::alloc(0); } && requires { impl::_dealloc(nullptr, 0); })
		,   "kh_allocator's implementation should not override alloc and dealloc and should instead provide a _alloc and _dealloc implementation"
		);

		static auto alloc(int size) -> void *
		{
			if constexpr (requires { impl::_alloc(size); })
				return impl::_alloc(size);
			return new char[size];
		}

		static auto dealloc(void * p, int size) -> bool
		{
			if constexpr (requires { impl::_dealloc(p, size); })
				return impl::_dealloc(p, size);

			delete[] reinterpret_cast<char *>(p);
			return true;
		}

		template <typename T>
		static auto talloc() -> T *
		{
			return reinterpret_cast<T *>(alloc(sizeof(T)));
		}

		template <typename T>
		static auto tdealloc(T * p) -> bool
		{
			return dealloc(p, sizeof(T));
		}
	};
}
