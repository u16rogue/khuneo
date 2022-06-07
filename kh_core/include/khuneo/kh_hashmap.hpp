#pragma once

#include <khuneo/defs.hpp>
#include <khuneo/xxhash.hpp>

namespace khuneo
{
	// TODO: test collissions - we can test how much prealloc we can do to minimize collissions as much as possible BEFORE we actually creating a new bucket
	// PLAN: p_bucket should be a ** so we can move each bucket entry without re alloc

	/*
	* Basic implementation of khuneo's hashmap
	*/
	#if 0
	template <typename key_t, typename value_t, unsigned int max_collission, unsigned int pre_alloc_bucket_count, typename kh_hashmap_impl_t>
	class kh_hashmap_basic
	{
		// Data type of the hash result of hashing a key_t using a key_hash_fn function from kh_hashmap_impl_t::kh_hash
		using key_hashed_t = typename kh_hashmap_impl_t::hash_t;
		using key_hash_fn  = key_hashed_t(*)(key_t);

		static_assert(requires { khuneo::kh_allocator_t _ = kh_hashmap_impl_t::kh_alloc; } && requires { khuneo::kh_deallocator_t _ = kh_hashmap_impl_t::kh_dealloc; }, "kh_hashmap_impl_t does not provide an alloc and dealloc implementation.");
		static_assert(requires { key_hashed_t(*_)(key_t)  = kh_hashmap_impl_t::kh_hash; }, "kh_hashmap_impl_t does not provide a valid hashing implementation");

		static constexpr khuneo::kh_allocator_t   kh_alloc   = kh_hashmap_impl_t::kh_alloc;
		static constexpr khuneo::kh_deallocator_t kh_dealloc = kh_hashmap_impl_t::kh_dealloc;
		static constexpr auto                     kh_hash    = kh_hashmap_impl_t::kh_hash;

		struct bucket_t 
		{
			key_hashed_t key;
			value_t      value;

			bucket_t * next;
		};

		auto bucket_bytes(int nbuckets) const -> int
		{
			return sizeof(bucket_t) * nbuckets;
		}

	public:
		kh_hashmap_basic()
		{
			grow(pre_alloc_bucket_count);
		}

		auto at(key_hashed_t hk) -> value_t *
		{
			#if 0
			bucket_t * r = &p_buckets[hash_to_index(hk)];
			if (r.)
			#endif
		}

	private:
		auto hash_to_index(key_hashed_t v) -> int
		{
			return v % bucket_count;
		}

		auto hash_to_index(key_hashed_t v, const int nbuckets) -> int
		{
			return v % nbuckets;
		}

		auto free_bucket(bucket_t * p, int nbuckets) -> bool
		{
			return kh_dealloc(p, bucket_bytes(nbuckets));
		}

		auto replace_bucket(bucket_t * pnew, int new_nbucket) -> bool
		{
			if (p_buckets && !free_bucket(p_buckets, bucket_count))
				return false;

			p_buckets    = pnew;
			bucket_count = new_nbucket;
			return true;
		}

		auto reloc_bucket(bucket_t * pnew, int new_nbucket) -> bool
		{
			return false;
		}

		auto resize(int nbuckets) -> bool
		{
			return false;
		};

		auto grow(int nbuckets) -> bool
		{
			return false;
		};

	private:
		int bucket_entries_count; // Number of occupied buckets
		int bucket_count; // Number of buckets allocated
		bucket_t ** p_buckets; // Pointer to the buckets
	};

	// TODO!
	struct kh_hashmap_xxh32_impl_t
	{
		static auto kh_alloc(int size) -> void * { return nullptr; };
		static auto kh_dealloc(void * p, int size) -> bool { return false; };

		using hash_t = khuneo::xxh32;
		static auto kh_hash(const char * str) -> khuneo::xxh32 { return khuneo::hash32(str); };
	};

	template <typename T>
	using kh_hashmap = kh_hashmap_basic<const char *, T, 4, 16, kh_hashmap_xxh32_impl_t>;
	#endif
}

