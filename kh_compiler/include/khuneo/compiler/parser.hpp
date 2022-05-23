#pragma once

#include <khuneo/compiler/ast.hpp>
#include <khuneo/compiler/lexer.hpp>
#include <khuneo/compiler/lang_impl.hpp>

namespace khuneo::parser
{
	template <typename... rules>
	auto basic_parse(impl::parser::info * info) -> bool
	{
		if (!info->state.source)
			info->state.source = info->ctx.start;

		if (!info->state.node)
			info->state.node = info->ctx.root_node;

		info->ctx.parser = basic_parse<rules...>;
 	
		// continue running while we're not at the end of the buffer
		while (!info->check_current_overflow(0))
		{
			bool has_exception = false;
			bool has_matched = ([&]
			{
				// Skip non parsable rules
				if constexpr (!requires { rules::parse(info); } )
					return false;

				bool matched = rules::parse(info);

				if (!matched && info->stack_find_recent(impl::parser::info_stack_type::EXCEPTION))
				{
					has_exception = true;
					return true;
				}

				// flush the stack
				while (info->stack_pop());

				return matched;
			}() || ...);

			if (!has_exception)
			{
				if (has_matched)
					continue;

				switch (*info->state.source)	
				{
					case ' ':
					case '\n':
					case '\t':
					case '\r':
						impl::lexer::forward_source<1>::parse(info);
						continue;
				}	
			}	

			if (!has_exception && info->state.source + 1 < info->ctx.end)
				info->generate_exception("Parser could not match any rule, expression, or token on the current source");

			break;
		}

		impl::parser::info_stack_entry * ce = info->stack_find_recent(impl::parser::info_stack_type::EXCEPTION);
		if (ce)
		{
			if (info->ctx.parser_exception)
				info->ctx.parser_exception(&ce->exception);
			return false;
		}

		return true;
	}

	template <typename... custom_rules>
	auto parse(impl::parser::info * info) -> bool
	{
		return basic_parse
		<	
			impl::lang::expr_endstatement,
			impl::lang::rule_moduleexport,
			impl::lang::rule_moduleimport,
			impl::lang::comment_line,
			impl::lang::comment_multi,
			impl::lang::rule_function,	
			impl::lang::rule_annotation,
			custom_rules...
		>(info);
	}
}