#pragma once

#include <khuneo/compiler/ast.hpp>
#include <khuneo/compiler/lexer.hpp>

namespace khuneo::impl::parser
{
	// Implements the parser rules
	template <khuneo::string_literal name = "SYMBOL">
	using symbol = lexer::kh_and
	<
		lexer::push_exception<"Expected a symbol/identifier">,
		lexer::h_match_AZaz_$,
		lexer::push_basic_state,
		lexer::forward_source<1>,
		lexer::h_gulp_char<lexer::h_match_AZaz_$09>,
		lexer::pop_token_next<name>,
		lexer::pop
	>;

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
			symbol<>,
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
			symbol<>,
			lexer::h_gulp_whitespace,
			lexer::kh_if<
				lexer::streq<"as">,
				lexer::push_basic_state,
				lexer::forward_source<>,
				lexer::pop_token_next<"IMPORT_ALIAS">,
				lexer::start_child,
					lexer::h_gulp_whitespace,
					symbol<>,
					lexer::h_gulp_whitespace,
				lexer::end_child
			>,
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

	using comment_line = lexer::kh_and
	<
		lexer::streq<"//">,
		lexer::kh_while<lexer::negate<lexer::streq<"\n">>, lexer::forward_source<1>>
	>;

	using comment_multi = lexer::kh_and
	<
		lexer::streq<"/*">,
		lexer::kh_while<lexer::negate<lexer::streq<"*/">>, lexer::forward_source<1>>
	>;

	using rule_function = lexer::kh_and
	<
		lexer::streq<"fn ">,
		lexer::push_basic_state,
		lexer::forward_source<>,
		lexer::pop_token_next<"FUNCTION">,
		lexer::h_gulp_whitespace,
		lexer::start_child,
			symbol<>,
			lexer::h_gulp_whitespace,
			lexer::push_exception<"Missing function parameter encapsulation '(' and ')'">,
			lexer::encapsulate<"(", ")">,
			lexer::push_basic_state,
			lexer::forward_source<>,
			lexer::pop_token_next<"FUNCTION_ARGS">,
			lexer::pop,
			lexer::h_gulp_whitespace,
			lexer::kh_if<lexer::streq<":">,
				lexer::forward_source<1>,
				lexer::h_gulp_whitespace,
				symbol<"TYPE">,
				lexer::h_gulp_whitespace
			>,
			lexer::push_exception<"Missing function body">,
			lexer::encapsulate<"{", "}">,
			lexer::push_basic_state,
			lexer::forward_source<>,
			lexer::pop_token_next<"BLOCK">,
			lexer::pop,
		lexer::end_child
	>;

	using rule_annotation = lexer::kh_and<
		lexer::streq<"@">,
		lexer::push_basic_state,
		lexer::forward_source<>,
		lexer::pop_token_next<"ANNOTATION">,
		lexer::start_child,
			symbol<>,
		lexer::end_child
	>;
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
 	
		// continue running while we're not at the end of the buffer
		while (!info->check_current_overflow(0))
		{
			bool has_exception = false;
			bool has_matched = ([&]
			{
				bool matched = rules::run(info);

				if (!matched && info->stack_find_recent(impl::info_stack_type::EXCEPTION))
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

				if (impl::lexer::h_spacingchars::run(info))
				{
					impl::lexer::forward_source<1>::run(info);
					continue;
				}
			}	

			if (!has_exception)
				info->generate_exception("Parser could not match any rule, expression, or token on the current source");

			break;
		}

		impl::info_stack_entry * ce = info->stack_find_recent(impl::info_stack_type::EXCEPTION);
		if (ce)
		{
			if (info->ctx.parser_exception)
				info->ctx.parser_exception(&ce->exception);
			return false;
		}

		return true;
	}

	template <typename... custom_rules>
	auto parse(impl::info * info) -> bool
	{
		return basic_parse
		<
			impl::parser::expr_endstatement,
			impl::parser::rule_moduleexport,
			impl::parser::rule_moduleimport,
			impl::parser::comment_line,
			impl::parser::comment_multi,
			impl::parser::rule_function,	
			impl::parser::rule_annotation,
			custom_rules...
		>(info);
	}
}