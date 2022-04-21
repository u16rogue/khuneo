#pragma once

#include <khuneo/compiler/ast.hpp>
#include <khuneo/compiler/lexer.hpp>

namespace khuneo::impl::parser
{
	// Implements the parser rules

	using expr_endstatement = lexer::kh_and
	<
		lexer::streq<";">,
		lexer::push_basic_state,
		lexer::forward_source<1>,
		lexer::pop_token_next<"END_STATEMENT">
	>;

	using rule_moduleexport = lexer::kh_and
	<
		lexer::streq<"export as ">,
		lexer::push_basic_state,
		lexer::forward_source<>,
		lexer::pop_token_next<"EXPORT_MODULE">,
		lexer::start_child,
			lexer::h_gulp_whitespace,
			lexer::symbol,
			lexer::h_gulp_whitespace,
			lexer::push_exception<"'export as' expected a property encapsulation '{' followed by a '}' or an end of statement ';'">,
			lexer::kh_or<
				expr_endstatement,
				lexer::kh_and<
					lexer::encapsulate<"{", "}">,
					lexer::push_basic_state,
					lexer::forward_source<>,
					lexer::pop_token_next<"EXPORT_PROPERTIES">
				>
			>,
			lexer::pop,
		lexer::end_child
	>;

	using rule_moduleimport = lexer::kh_and
	<
		lexer::streq<"import ">,
		lexer::push_basic_state,
		lexer::forward_source<>,
		lexer::pop_token_next<"IMPORT_MODULE">,
		lexer::start_child,
			lexer::h_gulp_whitespace,
			lexer::symbol,
			lexer::h_gulp_whitespace,
			lexer::push_exception<"'import' expected a property encapsulation '{' followed by a '}' or an end of statement ';'">,
			lexer::kh_or<
				expr_endstatement,
				lexer::kh_and<
					lexer::encapsulate<"{", "}">,
					lexer::push_basic_state,
					lexer::forward_source<>,
					lexer::pop_token_next<"IMPORT_PROPERTIES">
				>
			>,
			lexer::pop,
		lexer::end_child
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
						auto * ex = info->find_recent(impl::info_stack_type::EXCEPTION);
						if (ex)
						{
							// send the exception
							if (info->ctx.parser_exception)
								info->ctx.parser_exception(&ex->exception);
							return true;
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
			impl::parser::rule_moduleexport,
			impl::parser::rule_moduleimport,
			custom_rules...
		>(info);
	}
}