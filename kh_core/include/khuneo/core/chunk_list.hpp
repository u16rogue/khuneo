#pragma once

#include <khuneo/core/defs.hpp>

namespace khuneo::cont::details
{
	struct default_chunk_list_impl
	{
		using allocator = khuneo::details::kh_basic_allocator<>;
		static constexpr khuneo::u32 block_count = 8;
	};
}

namespace khuneo::cont/*ainer*/
{
	/*
	*  Linked list in group of chunks
	*/
	template <typename T, typename impl = details::default_chunk_list_impl>
	struct chunk_list
	{
		using self_t                     = chunk_list<T, impl>;
		using allocator                  = typename impl::allocator;	
		static constexpr khuneo::u32 block_count = impl::block_count;

		static auto construct(self_t * self) -> bool
		{
			self->ncount = 0;
			self->current_data = allocator::template talloc<chunk_group_t>();
			if (!self->current_data)
				return false;

			self->current_data->prev = nullptr;
			return true;
		}

		auto construct() -> bool { return construct(this); }

		// NOTE: This will only destroy the data blocks, if self is heap allocated you should free it yourself.
		// tl;dr destroy will only free the internal data not self itself
		static auto destroy(self_t * self) -> bool
		{
			chunk_group_t * c = self->current_data;
			while (c)
			{
				chunk_group_t * now = c;
				c = c->prev;

				if (!allocator::template tdealloc<chunk_group_t>(now))
					return false;
			}

			self->ncount       = 0;
			self->current_data = nullptr;
			return true;
		}

		auto destroy() -> bool { return destroy(this); }

		static auto append(self_t * self) -> T *
		{
			int new_count = self->ncount + 1;

			// Add new chunk if we need new chunks
			if (ncount_to_nchunk(new_count) != chunk_count(self))
			{
				chunk_group_t * new_chunk = allocator::template talloc<chunk_group_t>();
				if (!new_chunk)
					return nullptr;

				new_chunk->prev = self->current_data;
				self->current_data = new_chunk;
			}

			self->ncount = new_count;
			return &self->current_data->chunk_data[(new_count - 1) % block_count];
		}

		auto append() -> T * { return append(this); }

		static auto append_zeroed(self_t * self) -> T *
		{
			T * nblock = append(self);
			if (!nblock)
				return nullptr;

			for (int i = 0; i < sizeof(T); ++i)
				reinterpret_cast<khuneo::u8*>(nblock)[i] = 0;

			return nblock;
		}

		auto append_zeroed() -> T * { append_zeroed(this) }

		static auto count(self_t * self) -> khuneo::u32
		{
			return self->ncount;	
		}

		auto count() -> khuneo::u32 { count(this); }

		static auto get(self_t * self, khuneo::u32 index) -> T *
		{
			chunk_group_t * chunk = get_chunk(self, index_to_chunk_index(index));
			return &chunk->chunk_data[index % block_count];
		}

		auto get(khuneo::u32 index) -> T * { get(this, index); }

	private:
		struct chunk_group_t
		{
			T chunk_data[block_count];
			chunk_group_t * prev;
		};

		static auto ncount_to_nchunk(khuneo::u32 ncount) -> khuneo::u32
		{
			if (ncount <= block_count)
				return 1;

			khuneo::u32 n = ncount / block_count;
			if ((ncount % block_count) == 0)
				return n;

			return n + 1;
		}

		static auto chunk_count(self_t * self) -> khuneo::u32
		{
			return ncount_to_nchunk(self->ncount);
		}

		static auto index_to_chunk_index(khuneo::u32 i) -> khuneo::u32
		{
			return i / block_count;
		}

		static auto get_chunk(self_t * self, khuneo::u32 group_index) -> chunk_group_t *
		{
			khuneo::u32 nchunks = chunk_count(self) - 1;
			chunk_group_t * chunk = self->current_data;
			while (group_index != nchunks)
			{
				chunk = chunk->prev;
				--nchunks;
			}
			return chunk;
		}

		static auto get_first_chunk(self_t * self) -> chunk_group_t *
		{
			chunk_group_t * chunk = self->current_data;
			while (chunk->prev)
				chunk = chunk->prev;
			return chunk;
		}

		khuneo::u32 ncount;
		chunk_group_t * current_data;
	};
}