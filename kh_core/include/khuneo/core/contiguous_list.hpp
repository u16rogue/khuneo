#pragma once

#include <khuneo/core/defs.hpp>

namespace khuneo::cont::details
{
	struct default_contiguous_list_impl
	{
		using allocator = khuneo::details::kh_basic_allocator<>;
		static constexpr khuneo::u32 alloc_ahead_count = 8;
	};
}

namespace khuneo::cont
{
	template <typename T, typename impl = details::default_contiguous_list_impl>
	struct contiguous_list
	{
		using allocator = typename impl::allocator;
		using self_t = contiguous_list<T, impl>;
		static constexpr khuneo::u32 block_count = impl::alloc_ahead_count ? impl::alloc_ahead_count : 1;

		static auto construct(self_t * self) -> bool
		{
			self->data       = nullptr;
			self->used_count = 0;
			self->real_count = 0;
			return true;
		}

		auto construct() -> bool { return construct(this); }

		// This will only destroy the internal data, if self is heap allocated it must be free'd externally
		static auto destruct(self_t * self) -> bool
		{
			return allocator::dealloc(self->data, count_to_bytes(self->real_count));
		}

		auto destruct() -> bool { return destruct(this); }

		static auto get(self_t * self, khuneo::u32 index) -> T *
		{
			return &self->data[index];
		}

		auto get(khuneo::u32 index) -> T * { return get(this, index); }

		static auto count(self_t * self) -> khuneo::u32
		{
			return self->used_count;
		}

		auto count() -> khuneo::u32 { return count(this); }

		static auto append(self_t * self) -> T *
		{
			if (!grow(self, 1))
				return nullptr;

			T * d = &self->data[self->used_count];
			++self->used_count;
			return d;
		}

		auto append() -> T * { return append(this); }

	private:

		static auto count_to_bytes(khuneo::u32 n) -> khuneo::u32
		{
			return n * sizeof(T);
		}

		// Resizes data based off n. The value of n will be assigned
		// to real_count
		static auto resize_fixed_count(self_t * self, khuneo::u32 n) -> bool
		{
			// Alloc new
			T * new_data = reinterpret_cast<T *>(allocator::alloc(count_to_bytes(n)));
			if (!new_data)
				return false;

			if (self->data)
			{
				// Copy over data
				khuneo::u32 ncopy = self->used_count;
				for (khuneo::u32 i = 0; i < ncopy; ++i)
					new_data[i] = self->data[i];

				// Should panic here, new_data memory leak!
				// TODO: implement this better
				if (!allocator::dealloc(self->data, count_to_bytes(self->real_count)))
					return false;
			}

			self->data       = new_data;
			self->real_count = n;
			return true;
		}

		// Automatically grows data to upto count n, if there's space for
		// n blocks this function will return true, false indicates a failure
		// in allocating the required size to fit n blocks
		static auto grow(self_t * self, khuneo::u32 n) -> bool
		{
			if (self->used_count + n <= self->real_count)
				return true;

			khuneo::u32 need = (self->used_count + n) - self->real_count;
			return resize_fixed_count(self, self->real_count + (need + (block_count - (need % block_count))));
		}

	private:
		T * data;
		khuneo::u32 used_count;
		khuneo::u32 real_count;
	};
}