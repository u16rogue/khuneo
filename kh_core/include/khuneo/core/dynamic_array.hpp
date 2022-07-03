#pragma once

#include <khuneo/core/defs.hpp>

namespace khuneo::cont::details
{
	struct default_dynamic_array_impl
	{
		using allocator = khuneo::details::kh_allocator<>;
		static constexpr khuneo::u32 alloc_blocks = 8;
	};
}

namespace khuneo::cont/*ainers*/
{
	template <typename T, typename impl = details::default_dynamic_array_impl>
	struct dynamic_array
	{
		using self_t                 = dynamic_array<T, impl>;
		using allocator              = typename impl::allocator;
		static constexpr khuneo::u32 ablock = impl::alloc_blocks;

		static auto append(self_t * self) -> T *
		{
			if (!grow(self, 1))
				return false;

			return &self->data[self->ncount];
		}

		static auto get(self_t * self, int i) -> T *
		{
			return &self->data[i];
		}

		static auto count(self_t * self) -> khuneo::u32
		{
			return self->ncount;
		}

	private:
		static auto grow(self_t * self, int count) -> bool
		{

		}

		khuneo::u32 ncount;
		khuneo::u32 size;
		T * data;
	};
}