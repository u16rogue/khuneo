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
		lexer::kh_or<
			expr_endstatement,
			lexer::kh_and<
				lexer::encapsulate<"{", "}">,
				lexer::begin_token,
				lexer::forward_source<>,
				lexer::insert_token<"EXPORT_PROPERTIES">
			>
		>
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
			if ((rules::run(info) || ...))
				continue;

			// maybe we should fail instead
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