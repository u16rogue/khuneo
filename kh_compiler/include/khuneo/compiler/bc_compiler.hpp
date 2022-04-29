#pragma once

#include <khuneo/defs.hpp>
#include <khuneo/compiler/ast.hpp>

namespace khuneo::compiler
{
	struct bccomp_info
	{
		ast::node * ast_root_node;
		kh_allocator_t kh_alloc;
		kh_deallocator_t kh_free;
		kh_bytecode_t * bc_buffer;
		int bc_buffer_size;
	};

	auto bc_compiler(bccomp_info * pbcci) -> bool;
}