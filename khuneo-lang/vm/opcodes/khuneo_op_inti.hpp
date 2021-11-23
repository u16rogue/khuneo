#pragma once

#include "../khuneo_vm_ops.hpp"

namespace khuneo::vm::opcode
{
	// Intermidiate interrupt
	//		inti <interrupt code | 1 byte> <optional data>
	// Interrupt codes:
	// 
	//		'a' = Any interrupt | inti a
	//			> Nothing significant, can be used for any purpose. eg sending this interrupt
	//			  to do things from the interrupt handler
	//			> Invokes handler? Yes
	// 
	//		'm' = Message interrupt | inti m "message\0"
	//			> Sets the r0 register to the address of the null terminated cstring message
	//			> Invokes handler? Yes
	//
	using op_inti = impl::define_opcode<"inti", 1, [](KHUNEO_CTX_PARAM)
	{
		KHUNEO_CTX.registers.interrupt_flag = reinterpret_cast<const char *>(KHUNEO_CTX.registers.instruction_pointer)[4];

		// Process the interrupt request
		switch (KHUNEO_CTX.registers.interrupt_flag)
		{
			case 'a':
			{
				if (KHUNEO_CTX.interrupt_handler)
					KHUNEO_CTX.interrupt_handler(KHUNEO_CTX);

				KHUNEO_CTX.registers.instruction_pointer += 5;
				break;
			}

			case 'm':
			{
				char *message = reinterpret_cast<char *>(KHUNEO_CTX.registers.instruction_pointer + 5);
				KHUNEO_CTX.registers.r0.value.ptr = reinterpret_cast<decltype(KHUNEO_CTX.registers.r0.value.ptr)>(message);

				if (KHUNEO_CTX.interrupt_handler)
				{
					KHUNEO_CTX.interrupt_handler(KHUNEO_CTX);
				}
				else
				{
					if (KHUNEO_CTX.exception_handler)
						KHUNEO_CTX.exception_handler(KHUNEO_CTX, khuneo::vm::exceptions::NO_INTERRUPT_HANDLER);
				}

				// Advance the instruction pointer past the message string
				while (*message) ++message;
				KHUNEO_CTX.registers.instruction_pointer = reinterpret_cast<decltype(KHUNEO_CTX.registers.instruction_pointer)>(message + 1);
				break;
			}

			default:
			{
				if (KHUNEO_CTX.exception_handler)
					KHUNEO_CTX.exception_handler(KHUNEO_CTX, khuneo::vm::exceptions::INVALID_INTERRUPT_CODE);

				KHUNEO_CTX.registers.instruction_pointer = reinterpret_cast<decltype(KHUNEO_CTX.registers.instruction_pointer)>(-1);
				break;
			}
		}

		// Reset the interrupt flag
		KHUNEO_CTX.registers.interrupt_flag = 0;
	}>;
}