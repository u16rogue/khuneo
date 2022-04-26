#include <khuneo/runtime/vm.hpp>

auto khuneo::vm::vm_execute(vm_context * ctx) -> bool
{
	if (!ctx->current)
		ctx->current = ctx->start;

	while (ctx->current != ctx->end)
	{
		
	}

	return false;
}