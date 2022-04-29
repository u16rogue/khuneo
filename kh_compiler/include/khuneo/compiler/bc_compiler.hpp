#pragma once

#include <khuneo/compiler/ast.hpp>

namespace khuneo::compiler
{

	struct bccomp_info
	{
		ast::node * ast_root_node;

	};

	auto bc_compiler(bccomp_info * pbcci) -> bool;
}