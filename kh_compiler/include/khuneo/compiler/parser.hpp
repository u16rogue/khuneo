#pragma once

#include <khuneo/compiler/ast.hpp>
#include <khuneo/compiler/lexer.hpp>

namespace khuneo
{
	template <typename allocator, typename T_wc>
	struct parser
	{
		template <typename... toks>
		static auto basic_generate(khuneo::ast::ast_node * node, khuneo::impl::lexer::parse_context<T_wc> * pc, khuneo::impl::lexer::parse_response * resp) -> bool
		{
			khuneo::ast::ast_node * root_node = node;
			pc->current_node = root_node;

			while (pc->current < pc->end)
			{
				((toks::parse(pc, resp)) && ...);
			}

			return true;
		}

		static auto generate(khuneo::ast::ast_node * node, khuneo::impl::lexer::parse_context<T_wc> * pc, khuneo::impl::lexer::parse_response * resp) -> bool
		{
			using _t = typename khuneo::lexer<allocator>;
			return basic_generate<typename _t::const_mod, typename _t::var_decl>(node, pc, resp);
		}
	};
}