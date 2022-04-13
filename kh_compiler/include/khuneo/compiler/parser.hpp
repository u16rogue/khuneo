#pragma once

#include <khuneo/compiler/ast.hpp>
#include <khuneo/compiler/lexer.hpp>

namespace khuneo::impl::parser
{

}

namespace khuneo::parser
{
	template <typename... tokens>
	auto basic_parse(impl::info * info) -> bool
	{
		if (!info->state.source)
			info->state.source = info->ctx.start;

		if (!info->state.node)
			info->state.node = info->ctx.root_node;

		info->ctx.parser = basic_parse<tokens...>;

		while (!info->check_current_overflow(0))
		{
			if ((tokens::run(info) || ...))
				continue;
			
			// maybe we should fail instead
			impl::lexer::forward_source<1>::run(info);
		}

		return true;
	}

	template <typename... custom_tokens>
	auto parse(impl::info * info) -> bool
	{
		return basic_parse
		<
			lexer::expr_moduleexport,
			//lexer::importexpr,
			custom_tokens...
		>(info);
	}
}