#include <khuneo/runtime/vm.hpp>

auto khuneo::vm::vm_execute(vm_context * ctx) -> bool
{
	if (!ctx->registers.ip.v_p)
		ctx->registers.ip.v_p = ctx->start;

	while (ctx->registers.ip.v_p != ctx->end)
	{
		
	}

	return false;
}