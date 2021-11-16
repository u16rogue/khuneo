#pragma once

#include <cstdint>
#include <cstddef>

namespace khuneo::impl
{
	union kh_data_store
	{
		std::uint8_t * ptr;
		std::uintptr_t iptr;

		std::uint8_t   u8;
		std::uint16_t u16;
		std::uint32_t u32;
		std::uint64_t u64;
	};

	enum class symbol_type
	{
		INTEGER,
		STRING,
		FUNCTION
	};
	
	struct symbol_flags
	{
		bool is_ref    : 1; // If true, the actual data is stored somewhere else referenced (pointer) by data.ptr, otherwise if false the data is inside the data variable itself
		bool is_const  : 1; // Determines if its modifiable or not
		bool is_native : 1;
	};

	class symbol
	{
		const char * name;
		unsigned int hashed_name;
		int ref_count;

		kh_data_store data;
		std::size_t size;

		symbol_type type;
		symbol_flags flags;
	};
}