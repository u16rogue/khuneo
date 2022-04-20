#pragma once

#include <khuneo/compiler/ast.hpp>
#include <khuneo/compiler/lexer.hpp>

namespace khuneo::impl::parser
{
	// Implements the parser rules

	using expr_endstatement = lexer::kh_and
	<
		lexer::streq<";">,
		lexer::begin_token,
		lexer::forward_source<1>,
		lexer::insert_token<"END_STATEMENT">
	>;

	using expr_moduleexport = lexer::kh_and
	<
		lexer::streq<"export as ">,
		lexer::begin_token,
		lexer::forward_source<>,
		lexer::insert_token<"EXPORT_MODULE">,
		lexer::h_gulp_whitespace,
		lexer::symbol,
		lexer::h_gulp_whitespace,
		lexer::push_exception<"'export as' expected a property encapsulation '{' followed by a '}' or an end of statement ';'">,
		lexer::kh_or<
			expr_endstatement,
			lexer::kh_and<
				lexer::encapsulate<"{", "}">,
				lexer::begin_token,
				lexer::forward_source<>,
				lexer::insert_token<"EXPORT_PROPERTIES">
			>
		>,
		lexer::pop
	>;

	using comment_line = void;
	using comment_encap = void;
}

namespace khuneo::parser
{
	template <typename... rules>
	auto basic_parse(impl::info * info) -> bool
	{
		if (!info->state.source)
			info->state.source = info->ctx.start;

		if (!info->state.node)
			info->state.node = info->ctx.root_node;

		info->ctx.parser = basic_parse<rules...>;

		while (!info->check_current_overflow(0))
		{
			// If statement expects a catastrophic failure, true = fail, false = success
			if (([&]() -> bool
				{
					// for every rule run it and check if it fails, if there's
					// an exception on the stack, that means it wasn't a clean failure
					// which should indicate a compilation error
					if (!rules::run(info))
					{
						// look for an exception
						int sp = info->stack_count();
						while (sp)
						{
							auto & s = info->stack_indexed(sp);
							if (s.type == impl::info_stack_type::EXCEPTION)
							{
								// send the exception
								if (info->ctx.parser_exception)
									info->ctx.parser_exception(&s.exception);
								return true;
							}
							--sp;
						}
					}

					// flush the stack
					while (info->pop());

					return false;
				}() || ...)
			)
			{
				return false; // Catastrophic failure was met, exit the parser
			}
			
			// move on to the next if there were no exceptions
			impl::lexer::forward_source<1>::run(info);
		}

		return true;
	}

	template <typename... custom_rules>
	auto parse(impl::info * info) -> bool
	{
		return basic_parse
		<
			impl::parser::expr_moduleexport,
			custom_rules...
		>(info);
	}
}