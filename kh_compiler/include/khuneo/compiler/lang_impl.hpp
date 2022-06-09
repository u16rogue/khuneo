#pragma once

#include <khuneo/compiler/ast.hpp>
#include <khuneo/compiler/lexer.hpp>
#include <khuneo/runtime/vm.hpp>

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
	KHUNEO_DEF_TOKEN(EXPORT_MODULE);
	KHUNEO_DEF_TOKEN(IMPORT_MODULE);
	KHUNEO_DEF_TOKEN(FUNCTION);
	KHUNEO_DEF_TOKEN(FUNCTION_ARGS);
	KHUNEO_DEF_TOKEN(BLOCK);
	KHUNEO_DEF_TOKEN(TYPE);
	KHUNEO_DEF_TOKEN(ANNOTATION);
	KHUNEO_DEF_TOKEN(VARIABLE);
	KHUNEO_DEF_TOKEN(ASSIGNMENT_EXPR);
};

namespace khuneo::impl::lang
{
	// Implements the parser rules
	template <khuneo::string_literal name = toks::SYMBOL, khuneo::string_literal ex_msg = "Expected a symbol/identifier">
	using symbol = lexer::kh_and
	<
		lexer::push_exception<ex_msg>,
		lexer::h_match_AZaz_$,
		lexer::push_basic_state,
		lexer::forward_source<1>,
		lexer::h_gulp_char<lexer::h_match_AZaz_$09>,
		lexer::pop_token_next<name>,
		lexer::pop
	>;

	using type = symbol<toks::TYPE, "Expected a type name">;

	

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
	
	struct expr_assignment
	{
		static auto parse(impl::parser::info * i) -> bool
		{
			return lexer::kh_and
			<
				lexer::streq<"=">,
				lexer::forward_source<>,
				lexer::h_gulp_whitespace,
				lexer::push_basic_state,
				lexer::kh_while<lexer::kh_and<lexer::negate<lexer::streq<";">>, lexer::forward_source<1>>>,
				lexer::pop_token_next<toks::ASSIGNMENT_EXPR>
			>::parse(i);
		}
	};

	using parse_child_sym_assignment = lexer::parse_child<0, lexer::kh_and
	<
		lexer::h_gulp_whitespace,
		symbol<>,
		lexer::h_gulp_whitespace,
		lexer::kh_if<lexer::streq<"=">, expr_assignment>
	>>;

	using comma_separator = lexer::kh_or
	<
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

	struct rule_variable
	{
		static auto parse(impl::parser::info * i) -> bool
		{
			return lexer::kh_and
			<
				lexer::streq<"let ">,
				lexer::push_basic_state,
				lexer::forward_source<>,
				lexer::pop_token_next<toks::VARIABLE>,
				lexer::start_child,
					lexer::h_gulp_whitespace,
					symbol<>,
					lexer::kh_if<lexer::streq<":">,
						lexer::forward_source<>,
						lexer::h_gulp_whitespace,
						type
					>,
					lexer::h_gulp_whitespace,
					lexer::push_exception<"Expected assignment or end of statement">,
					lexer::kh_or
					<
						expr_endstatement,
						lexer::kh_and<expr_assignment, expr_endstatement>
					>,
				lexer::end_child
			>::parse(i);
		}

		static auto compile(impl::compiler::bccomp_info * i) -> bool;
	};

	struct rule_moduleexport 
	{
		static auto parse(impl::parser::info * i) -> bool
		{
			return lexer::kh_and
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
							lexer::pop_token_next<toks::BLOCK>,
							lexer::parse_child<1, comma_separator>
						>
					>,
					lexer::pop,
				lexer::end_child
			>::parse(i);
		}

		static auto compile(impl::compiler::bccomp_info * i) -> bool;
	};

	struct rule_moduleimport 
	{
		static auto parse(impl::parser::info * i) -> bool {
			return lexer::kh_and
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
						lexer::streq<"as ">,
						lexer::forward_source<>,
						symbol<>
					>,
					lexer::push_exception<"'import' expected a property encapsulation '{' followed by a '}' or an end of statement ';'">,
					lexer::kh_or<
						expr_endstatement,
						lexer::kh_and<
							lexer::h_gulp_whitespace,
							lexer::encapsulate<"{", "}">,
							lexer::push_basic_state,
							lexer::forward_source<>,
							lexer::pop_token_next<toks::BLOCK>
						>
					>,
					lexer::pop,
				lexer::end_child
			>::parse(i);
		}
	};

	using comment_line = lexer::kh_and
	<
		lexer::streq<"//">,
		lexer::kh_while<lexer::kh_and<lexer::negate<lexer::streq<"\n">>, lexer::negate<lexer::check_end<0>>>, lexer::forward_source<1>>
	>;

	using comment_multi = lexer::kh_and
	<
		lexer::streq<"/*">,
		lexer::forward_source<2>,
		lexer::kh_while<
			lexer::kh_and<
				lexer::negate<lexer::streq<"*/">>,
				lexer::negate<lexer::check_end<0>>
			>,
			lexer::forward_source<1>
		>,
		lexer::forward_source<2>
	>;

	struct rule_function
	{
		static auto parse(impl::parser::info * i) -> bool
		{
			return lexer::kh_and
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
			>::parse(i);
		}
	};

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

	struct rule_annotation
	{
		static auto parse(impl::parser::info * i) -> bool
		{
			return lexer::kh_and<
				lexer::streq<"@">,
				lexer::push_basic_state,
				lexer::forward_source<>,
				lexer::pop_token_next<toks::ANNOTATION>,
				lexer::start_child,
				symbol<>,
				lexer::end_child
			>::parse(i);
		}
	};

	// TODO: maybe have a single struct to stand as a type that loads all the language implementation
}