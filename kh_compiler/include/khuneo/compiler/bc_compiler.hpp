#pragma once

#include <khuneo/defs.hpp>
#include <khuneo/compiler/ast.hpp>

namespace khuneo
{
	enum class bc_compiler_exception_type
	{
		WARNING,
		FATAL
	};

	struct bc_compiler_exception
	{
		bc_compiler_exception_type type;
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
		fn_bccomp_except_t bc_except;
		kh_bytecode_t * bc_buffer;
		kh_bytecode_t * bc_current;
		kh_bytecode_t * bc_end;
	};

	auto bc_compile(bccomp_info * pbcci) -> bool;
}