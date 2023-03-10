#pragma once

#include <khuneo/core/metapp.hpp>

namespace khuneo {
#define KH_DEFPRIMT(bitsz, signed_t)  \
  using i##bitsz = signed_t;          \
  using u##bitsz = unsigned signed_t; \
  static_assert(sizeof(u##bitsz) == bitsz / 8 && sizeof(i##bitsz) == bitsz / 8, "[khuneo] type did not match the expected size.")

KH_DEFPRIMT(8 , char     ); 
KH_DEFPRIMT(16, short    );
KH_DEFPRIMT(32, int      );
KH_DEFPRIMT(64, long long);

using f32 = float;
using f64 = double;

#undef KH_DEFPRIMT
} // namespace khuneo

namespace khuneo::details {
struct kh_default_std_allocator {
  static auto _alloc(int sz) -> void * { return new char[sz]; };
  static auto _dealloc(void * p, int sz) -> bool {
    delete[] (char *)p;
    return true;
  };
};

/*
 * kh_basic_allocator - Allocation scheme implementation used by khuneo.
 *
 * -- Implementing your own:
 *		You can implement your own allocation method by creating your
 *		own class and inheriting kh_basic_allocator in a CRTP pattern ( class myalloc : kh_basic_allocator<myalloc> )
 *		and providing a _alloc and _dealloc implementation. The prototype for the following:
 *			alloc   - static void * _alloc(int size);
 *			dealloc - static bool _dealloc(void * p, int size);
 */
template <typename impl = metapp::details::invalid_type>
struct kh_basic_allocator {
  static constexpr auto is_invalid() -> bool { return metapp::is_t_invalid<impl>::value; };

  static_assert(
    !metapp::is_t_invalid<impl>::value && requires { impl::_alloc(0); }, "kh_basic_allocator's implementation must provide a [static void * _alloc(int size)] implementation");

  static_assert(
    !metapp::is_t_invalid<impl>::value && requires { impl::_dealloc(nullptr, 0); }, "kh_basic_allocator's implementation must provide a [static bool _dealloc(void * p, int size)] implementation");

	// If either alloc and/or delloc was provided by the implementation turn it to false to cause an error
  static_assert(
    !metapp::is_t_invalid<impl>::value && !(
																						requires { impl::alloc(0); } || requires { impl::dealloc(nullptr, 0); }) ,
																						"kh_basic_allocator's implementation should not override alloc and dealloc and should instead provide a _alloc and _dealloc implementation"
																					 );

  static auto alloc(int size) -> void * {
    return impl::_alloc(size);
  }

  static auto dealloc(void * p, int size) -> bool {
    return impl::_dealloc(p, size);
  }

  template <typename T>
  static auto talloc() -> T * {
    return reinterpret_cast<T *>(alloc(sizeof(T)));
  }

  template <typename T>
  static auto tdealloc(T * p) -> bool {
    return dealloc(p, sizeof(T));
  }
};

// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#if 0
	template <typename impl, typename allocator>
	struct kh_basic_container
	{
		using T = impl;

		// Creates an impl instance, this will automatically call construct
		static auto create() -> T *
		{
			T * i = allocator::template talloc<T>();
			if (!i)
				return nullptr;

			if (!construct(i))
			{
				allocator::template tdealloc<T>(i);
				return nullptr;
			}

			return i;
		}

		// Destroys an impl instance, this will automatically call destruct
		static auto destroy(T * i) -> bool
		{
			if (!destruct(i))
				return false;

			return allocator::template tdealloc<T>(i);
		}

		static auto construct(T * i) -> bool
		{
			return i->construct();
		}

		static auto destruct(T * i) -> bool
		{
			return i->destruct();
		}

		static auto count(T * i) -> khuneo::u32
		{
			return i->count();
		}
	};
#endif
} // namespace khuneo::details
