#pragma once

#include <khuneo/compiler/info.hpp>
#include <khuneo/compiler/lang_impl.hpp>

namespace khuneo::compiler
{
	template <typename... rules>
	auto bc_compile_basic(impl::compiler::bccomp_info * pbcci) -> bool
	{
		if (!pbcci || !pbcci->kh_alloc || !pbcci->kh_dealloc || !pbcci->ast_root_node)
			return false;

		if (!pbcci->current_node)
			pbcci->current_node = pbcci->ast_root_node;

		ast::node * prev_node = nullptr;
		while (pbcci->current_node)
		{
			prev_node = pbcci->current_node;

			bool has_matched = ([&]
			{
				if constexpr (requires { rules::compile(pbcci); })
					return rules::compile(pbcci);
				return false;
			} () || ...);

			if (!has_matched && pbcci->bc_except)
			{
				khuneo::bc_compiler_exception bce {};
				bce.message = "No compiler rule matched the provided AST node.";
				bce.type = khuneo::bc_compiler_exception_type::WARNING;
				bce.node = pbcci->current_node;
				pbcci->bc_except(&bce);
			}

			// If a compile rule did not move the current node, we move it ourselves
			// this should allow compile rules to move the nodes by themselves
			// without disrupting the state only to move it when needed
			if (pbcci->current_node == prev_node)
				pbcci->current_node = prev_node->next;
		}

		return true;
	}

	template <typename... custom_rules>
	auto bc_compile(impl::compiler::bccomp_info * pbcci) -> bool
	{
		return bc_compile_basic<
			impl::lang::rule_moduleexport,
			impl::lang::rule_moduleimport,
			impl::lang::rule_variable,
			custom_rules...	
		>(pbcci);
	}
}