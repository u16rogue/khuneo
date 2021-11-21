#pragma once

#include "khuneo_vm_ops.hpp"
#include "opcodes/khuneo_opcodes.hpp"

namespace khuneo::vm::impl
{
	template <class... opcodes>
	auto basic_execute(KHUNEO_CTX_PARAM, void * code, void * eoc) -> bool
	{
		static_assert(!khuneo::vm::impl::opcode_collision_check::run<opcodes...>(), "Static check failed due to an opcode collision! This is caused by an opcode generating the same code value for the VM.");

		// Initialize the context
		KHUNEO_CTX.registers.instruction_pointer = reinterpret_cast<decltype(KHUNEO_CTX.registers.instruction_pointer)>(code);
		KHUNEO_CTX.stack_first             = &KHUNEO_CTX.stack[0];
		KHUNEO_CTX.stack_last              = &KHUNEO_CTX.stack[sizeof(KHUNEO_CTX.stack) / sizeof(KHUNEO_CTX.stack[0])];
		KHUNEO_CTX.registers.stack_pointer = KHUNEO_CTX.stack_last;

		while (KHUNEO_CTX.registers.instruction_pointer < eoc)
		{
			if ((opcodes::check_and_exec(KHUNEO_CTX) || ...) == false)
			{
				if (KHUNEO_CTX.exception_handler)
					KHUNEO_CTX.exception_handler(KHUNEO_CTX, khuneo::vm::exceptions::INVALID_OPCODE);

				return false;
			}
		}

		return true;
	}
}

namespace khuneo::vm
{
	template <class... custom_opcodes>
	auto execute(KHUNEO_CTX_PARAM, void * code, void * eoc) -> bool
	{
		return impl::basic_execute<
			// Default opcodes used by the VM
			khuneo::vm::opcode::op_nop,
			khuneo::vm::opcode::op_inti,
			// Add custom opcodes
			custom_opcodes...
		>(KHUNEO_CTX, code, eoc);
	}
}