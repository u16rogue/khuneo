#pragma once

#include <cstdint>
#include <cstddef>

namespace khuneo::impl
{
	enum class kh_managed_type
	{
		INTEGER,
		STRING,
		FUNCTION,
		STRUCTURE
	};

	struct kh_managed_value_block
	{
		int             ref_count { 0 };
		kh_managed_type type;
		std::size_t     size;
		char            buffer[];
	};

	union kh_data_store
	{
		kh_managed_value_block * managed_block;

		std::uint8_t * ptr;
		std::uintptr_t iptr;

		std::uint8_t   u8;
		std::uint16_t u16;
		std::uint32_t u32;
		std::uint64_t u64;
	};

	enum class symbol_type
	{
		MANAGED,    // symbol::data is a pointer to a kh_managed_value_block
		INPLACE,    // symbol::data contains the value itself
		NATIVE_REF, // symbol::data is a pointer to an unmanaged memory
	};

	class symbol
	{
		const char * name;
		unsigned int hashed_name;

		symbol_type   type;
		kh_data_store data;
	};
}