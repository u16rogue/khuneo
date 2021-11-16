#pragma once

#include <cstdint>

#include "khuneo_state.hpp"
#include "khuneo_data.hpp"

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
	enum class ctx_register_type
	{
		SYMBOL, // The current value of the register is a symbol
		NUMBER  // The current value of the register is a value itself
	};

	struct ctx_register
	{
		union
		{
			std::uint8_t         * ptr;
			khuneo::impl::symbol * symbol;
			double                 number;
		} value;

		ctx_register_type contains;
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
			std::uint8_t * ip   { 0 }; // instruction pointer
			char interrupt_flag { 0 };

			union
			{
				struct
				{
					ctx_register r0;
					ctx_register r1;
					ctx_register r2;
					ctx_register r3;
					ctx_register r4;
					ctx_register r5;
					ctx_register r6;
					ctx_register r7;
					ctx_register r8;
					ctx_register r9;
					ctx_register r10;
					ctx_register r11;
					ctx_register r12;
					ctx_register r13;
					ctx_register r14;
					ctx_register r15;
				};

				ctx_register r[16];
			};
		} registers;
	};
}