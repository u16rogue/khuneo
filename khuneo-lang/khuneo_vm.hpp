#pragma once

#include "khuneo_def.hpp"
#include "khuneo_vm_ops.hpp"

namespace khuneo::vm::impl
{
	template <class... opcodes>
	auto basic_execute(KHUNEO_CTX_PARAM, void * code, void * eoc) -> bool
	{
		static_assert(!khuneo::vm::impl::opcode_collision_check::run<opcodes...>(), "Static check failed due to an opcode collision! This is caused by an opcode generating the same code value for the VM.");

		// Initialize the context
		KHUNEO_CTX.registers.ip.ptr = reinterpret_cast<decltype(KHUNEO_CTX.registers.ip.ptr)>(code);

		while (KHUNEO_CTX.registers.ip.ptr < eoc)
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
	auto execute(KHUNEO_CTX_PARAM, void * code, void * eoc) -> void
	{
		impl::basic_execute<
			// Default opcodes used by the VM
			khuneo::vm::codes::op_nop,
			khuneo::vm::codes::op_interrupt_imm,
			// Add custom opcodes
			custom_opcodes...
		>(KHUNEO_CTX, code, eoc);
	}
}