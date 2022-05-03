#pragma once

#include <khuneo/defs.hpp>
#include <khuneo/compiler/ast.hpp>

namespace khuneo
{
	struct bc_compiler_exception
	{
		ast::node * node;
		const char * message;
	};
}

namespace khuneo::compiler
{
	using fn_bccomp_except_t = void(*)(khuneo::bc_compiler_exception *); 
	
	struct bccomp_info
	{
		ast::node * ast_root_node;
		kh_allocator_t kh_alloc;
		kh_deallocator_t kh_free;
		kh_bytecode_t * bc_buffer;
		fn_bccomp_except_t bc_except;
		int bc_buffer_size;
	};

	auto bc_compile(bccomp_info * pbcci) -> bool;
}