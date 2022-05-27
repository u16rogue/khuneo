#pragma once

#include <khuneo/compiler/info.hpp>
#include <khuneo/compiler/lang_impl.hpp>

namespace khuneo::compiler
{
	template <typename... rules>
	auto bc_compile_basic(impl::compiler::bccomp_info * pbcci) -> bool
	{
		ast::node * cnode = nullptr; // current node 	

		if (!pbcci || !pbcci->kh_alloc || !pbcci->kh_free || !pbcci->ast_root_node)
			return false;	

		

		return true;
	}


	template <typename... custom_rules>
	auto bc_compile(impl::compiler::bccomp_info * pbcci) -> bool
	{
		return bc_compile_basic<
			impl::lang::rule_moduleexport,
			custom_rules...	
		>(pbcci);
	}
}