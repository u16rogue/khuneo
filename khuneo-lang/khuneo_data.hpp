#pragma once

#include <cstdint>
#include <cstddef>

namespace khuneo::impl
{
	enum class kh_tag
	{
		INVALID,
		MANAGED_BLOCK,
		INPLACE,
		NATIVE_REF,
		NUMBER,
		STRING,
		FUNCTION,
		STRUCTURE
	};

	struct kh_managed_block;

	union kh_data
	{
		std::uint8_t * ptr;
		std::uintptr_t iptr;

		std::uint8_t   u8;
		std::uint16_t u16;
		std::uint32_t u32;
		std::uint64_t u64;
		
		kh_managed_block * kh_managed_block;
		char               kh_buffer[]; 
		double             kh_number;
	};

	// Used for dynamically loaded data
	struct kh_managed_block
	{
		int         ref_count   { 0 };
		kh_tag      tag         { kh_tag::INVALID };
		std::size_t buffer_size { sizeof(buffer)  };
		kh_data     buffer      { 0 };

		auto get_block_size() -> std::size_t
		{
			return sizeof(kh_managed_block) - sizeof(decltype(buffer)) + buffer_size;
		}
	};

	// Used for storing references or data
	struct kh_symbol
	{
		const char * name;
		unsigned int hashed_name;

		kh_tag  tag;
		kh_data value;
	};

	struct kh_stack_entry
	{
		kh_tag  tag;
		kh_data value;
	};
}