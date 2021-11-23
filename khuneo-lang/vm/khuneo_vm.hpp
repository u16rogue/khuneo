#pragma once

#include "khuneo_vm_ops.hpp"
#include "opcodes/khuneo_opcodes.hpp"

namespace khuneo::impl
{
	template <class... opcodes>
	auto basic_vm_execute(KHUNEO_CTX_PARAM, void * code, void * eoc) -> bool
	{
		static_assert(!khuneo::impl::opcode_collision_check::run<opcodes...>(), "Static check failed due to an opcode collision! This is caused by an opcode generating the same code value for the VM.");

		// Initialize the context
		KHUNEO_CTX.registers.instruction_pointer = reinterpret_cast<decltype(KHUNEO_CTX.registers.instruction_pointer)>(code);
		KHUNEO_CTX.stack_first             = &KHUNEO_CTX.stack[0];
		KHUNEO_CTX.stack_last              = &KHUNEO_CTX.stack[(sizeof(KHUNEO_CTX.stack) / sizeof(KHUNEO_CTX.stack[0])) - 1];
		KHUNEO_CTX.registers.stack_pointer = KHUNEO_CTX.stack_last;

		while (KHUNEO_CTX.registers.instruction_pointer < eoc)
		{
			if ((opcodes::check_and_exec(KHUNEO_CTX) || ...) == false)
			{
				if (KHUNEO_CTX.exception_handler)
					KHUNEO_CTX.exception_handler(KHUNEO_CTX, khuneo::exceptions::INVALID_OPCODE);

				return false;
			}
		}

		return true;
	}
}

namespace khuneo::impl
{
	template <class... custom_opcodes>
	auto vm_execute(KHUNEO_CTX_PARAM, void * code, void * eoc) -> bool
	{
		return khuneo::impl::basic_vm_execute<
			// Default opcodes used by the VM
			khuneo::impl::op_nop,
			khuneo::impl::op_inti,
			// Add custom opcodes
			custom_opcodes...
		>(KHUNEO_CTX, code, eoc);
	}
}