#pragma once

#include <cstdint>
#include "khuneo_state.hpp"

namespace khuneo::vm
{
	enum class exceptions
	{
		INVALID_OPCODE,
		NO_INTERRUPT_HANDLER,
		INVALID_INTERRUPT_CODE,
	};
}

namespace khuneo::vm::impl
{
	union context_storage
	{
		std::uint8_t * ptr;
		std::uintptr_t iptr;

		std::uint8_t   u8;
		std::uint16_t u16;
		std::uint32_t u32;
		std::uint64_t u64;
	};

	// The current context that the VM uses.
	// Similar to the concept of where each thread in a system has its own state, storage, registers, etc...
	struct context
	{
		khuneo::state * state { nullptr };
		void(*interrupt_handler)(context &);
		void(*exception_handler)(context &, exceptions);

		struct
		{
			context_storage ip  { 0 }; // instruction pointer
			char interrupt_flag { 0 };

			union
			{
				struct
				{
					context_storage r0; // General register 0
					context_storage r1; // General register 1
					context_storage r2; // General register 2
					context_storage r3; // General register 3
				};

				context_storage r[];
			};
		} registers;
	};

	struct register_id
	{
		enum
		{

		};
	};
}