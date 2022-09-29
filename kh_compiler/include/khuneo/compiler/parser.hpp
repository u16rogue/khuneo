#pragma once

#include <khuneo/compiler/lexer.hpp>

namespace khuneo::compiler::parser::details
{
	struct default_parser_impl
	{
		using lexer_impl = khuneo::compiler::lexer::details::default_lexer_impl;
	};
}

namespace khuneo::compiler::parser
{
	template <typename parser_impl>
	auto run() -> bool
	{
		return false;
	}
}