#pragma once

#include "../khuneo_vm_ops.hpp"

namespace khuneo::impl
{
	// No operation
	using op_nop = impl::define_opcode<"nop", 0, [](KHUNEO_CTX_PARAM)
	{
		KHUNEO_CTX.registers.instruction_pointer += 4;
	}>;
}