#pragma once

namespace khuneo::vm
{
	struct vm_context
	{
		unsigned char * start;
		unsigned char * current;
		unsigned char * end;
	};

	auto vm_execute(vm_context * ctx) -> bool;
}