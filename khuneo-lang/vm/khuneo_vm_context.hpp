#pragma once

#include <cstdint>

#include "../khuneo_state.hpp"
#include "../khuneo_data.hpp"

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
	struct context_register
	{
		khuneo::impl::kh_tag  tag;
		khuneo::impl::kh_data value;
	};

	struct context
	{
		khuneo::state * state { nullptr };
		void(*interrupt_handler)(context &);
		void(*exception_handler)(context &, exceptions);

		khuneo::impl::kh_stack_entry * stack_last;    // Last element of stack (&buffer[last_index])
		khuneo::impl::kh_stack_entry * stack_first;   // Top element of stack (&buffer[0])
		khuneo::impl::kh_stack_entry   stack[256]; // This is temporary, will (maybe) dynamically allocate stack in the future and support resizing.

		struct
		{
			std::uint8_t                 * instruction_pointer { 0 };
			khuneo::impl::kh_stack_entry * stack_pointer; // Current stack pointer (initialized to be lastof_stackbuffer)
			char interrupt_flag { 0 };

			union
			{
				struct
				{
					context_register r0;
					context_register r1;
					context_register r2;
					context_register r3;
					context_register r4;
					context_register r5;
					context_register r6;
					context_register r7;
					context_register r8;
					context_register r9;
					context_register r10;
					context_register r11;
					context_register r12;
					context_register r13;
					context_register r14;
					context_register r15;
				};

				context_register r[16];
			};
		} registers;
	};
}