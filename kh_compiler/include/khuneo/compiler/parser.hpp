#pragma once

#include <khuneo/compiler/ast.hpp>
#include <khuneo/compiler/lexer.hpp>

// TODO: get rid of X_PROPRTIES and just use a unified BLOCK or something similar
// TODO: create a token alias for encapsulation in the context of a parser

#define KHUNEO_DEF_TOKEN(n) \
	constexpr khuneo::string_literal n = #n 

// Not really necessary but this is nicer to have all of the available tokens in one place
// works well with code completion
namespace khuneo::impl::toks
{
	KHUNEO_DEF_TOKEN(ROOT_NODE);
	KHUNEO_DEF_TOKEN(SYMBOL);
	KHUNEO_DEF_TOKEN(COMMA_SEPARATED_GROUP);
	KHUNEO_DEF_TOKEN(END_STATEMENT);
	KHUNEO_DEF_TOKEN(EXPORT_PROPERTIES);
	KHUNEO_DEF_TOKEN(EXPORT_MODULE);
	KHUNEO_DEF_TOKEN(IMPORT_PROPERTIES);
	KHUNEO_DEF_TOKEN(IMPORT_ALIAS);
	KHUNEO_DEF_TOKEN(IMPORT_MODULE);
	KHUNEO_DEF_TOKEN(FUNCTION);
	KHUNEO_DEF_TOKEN(FUNCTION_ARGS);
	KHUNEO_DEF_TOKEN(BLOCK);
	KHUNEO_DEF_TOKEN(TYPE);
	KHUNEO_DEF_TOKEN(ANNOTATION);
};

namespace khuneo::impl::parser
{
	// Implements the parser rules
	template <khuneo::string_literal name = toks::SYMBOL>
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

	using group_parenthesis  = lexer::encapsulate<"(", ")">;
	using group_brackets     = lexer::encapsulate<"[", "]">;
	using group_curlybrace   = lexer::encapsulate<"{", "}">;
	using group_double_quote = lexer::encapsulate<"\"", "\"">;
	using group_singlequote  = lexer::encapsulate<"'", "'">;

	using group_either = lexer::kh_or
	<
		group_parenthesis,
		group_brackets,
		group_curlybrace,
		group_double_quote,
		group_singlequote
	>;
	
	using expr_assignment = lexer::kh_and
	<
		lexer::streq<"=">,
		lexer::forward_source<>,
		lexer::h_gulp_whitespace,
		lexer::push_basic_state,
		lexer::kh_while<lexer::kh_and<lexer::negate<lexer::streq<";">>, lexer::forward_source<1>>>,
		lexer::pop_token_next<"ASSIGNMENT_EXPR">
	>;

	using parse_child_sym_assignment = lexer::parse_child<0, lexer::kh_and
	<
		lexer::h_gulp_whitespace,
		symbol<>,
		lexer::h_gulp_whitespace,
		lexer::kh_if<lexer::streq<"=">, expr_assignment>
	>>;

	using comma_separator = lexer::kh_or<
		// This checks if the encapsulation actually has something
		// this will cause a short circuit causing the main comma
		// separator to not run
		lexer::kh_and
		<
			lexer::h_gulp_whitespace,
			lexer::check_end<1>
		>,
		lexer::kh_and
		<	
			lexer::push_basic_state,
			lexer::kh_while<lexer::forward_source<1>,	
				lexer::kh_or<
					lexer::kh_and< group_either,
						lexer::forward_source<0, -1>
					>,
					lexer::kh_if< lexer::streq<",">,
						lexer::pop_token_next<toks::COMMA_SEPARATED_GROUP>,
						parse_child_sym_assignment, 
						lexer::forward_source<1>,
						lexer::push_basic_state
					>	
				>
			>,
			lexer::pop_token_next<toks::COMMA_SEPARATED_GROUP>,
			parse_child_sym_assignment
		>
	>;
	
	using expr_endstatement = lexer::kh_and
	<
		lexer::streq<";">,
		lexer::push_basic_state,
		lexer::forward_source<1>,
		lexer::pop_token_next<toks::END_STATEMENT>
	>;

	using rule_moduleexport = lexer::kh_and
	<
		lexer::streq<"export as ">,
		lexer::push_basic_state,
		lexer::forward_source<>,
		lexer::pop_token_next<toks::EXPORT_MODULE>,
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
					lexer::pop_token_next<toks::EXPORT_PROPERTIES>,
					lexer::parse_child<1, comma_separator>
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
		lexer::pop_token_next<toks::IMPORT_MODULE>,
		lexer::start_child,
			lexer::h_gulp_whitespace,
			symbol<>,
			lexer::h_gulp_whitespace,
			lexer::kh_if<
				lexer::streq<"as">,
				lexer::push_basic_state,
				lexer::forward_source<>,
				lexer::pop_token_next<toks::IMPORT_ALIAS>,
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
					lexer::pop_token_next<toks::IMPORT_PROPERTIES>
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
		lexer::pop_token_next<toks::FUNCTION>,
		lexer::h_gulp_whitespace,
		lexer::start_child,
			symbol<>,
			lexer::h_gulp_whitespace,
			lexer::push_exception<"Missing function parameter encapsulation '(' and ')'">,
			group_parenthesis,
			lexer::push_basic_state,
			lexer::forward_source<>,
			lexer::pop_token_next<toks::FUNCTION_ARGS>,
			lexer::pop,
			lexer::h_gulp_whitespace,
			lexer::kh_if<lexer::streq<":">,
				lexer::forward_source<1>,
				lexer::h_gulp_whitespace,
				symbol<toks::TYPE>,
				lexer::h_gulp_whitespace
			>,
			lexer::push_exception<"Missing function body">,
			group_curlybrace,	
			lexer::push_basic_state,
			lexer::forward_source<>,
			lexer::pop_token_next<toks::BLOCK>,
			lexer::parse_child<>,
			lexer::pop,
		lexer::end_child
	>;

	#if 0
	using rule_returnkw = lexer::kh_and
	<
		lexer::streq<"return ">,
		lexer::push_basic_state,
		lexer::forward_source<>,
		lexer::pop_token_next<"FUNCTION_RETURN">
		lexer::start_child,
				
		lexer::end_child
	>;
	#endif

	using rule_annotation = lexer::kh_and<
		lexer::streq<"@">,
		lexer::push_basic_state,
		lexer::forward_source<>,
		lexer::pop_token_next<toks::ANNOTATION>,
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
				bool matched = rules::parse(info);

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