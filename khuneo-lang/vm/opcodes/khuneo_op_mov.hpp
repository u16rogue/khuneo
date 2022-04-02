#pragma once

#include "../khuneo_vm_ops.hpp"

namespace khuneo::impl
{
	// Move operation
	using op_mov = impl::define_opcode<"mov", 1, [](KHUNEO_CTX_PARAM)
	{

	}>;
}