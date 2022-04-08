#pragma once

#include <khuneo/compiler/parser.hpp>

namespace khuneo::lexer::tokens
{
	// Match token (No side effects)
	using whitespace = parser::any<" ", "\n", "\r", "\t">;

	// Token of valid names that are alphabetical and are symbols
	using name_alpha_sym_chars = parser::kh_or< parser::range<'A', 'Z'>, parser::range<'a', 'z'>, parser::any<"$", "_"> >;

	// This is a consumer token (Causes side effects) parser::kh_or<parser::range<'A', 'Z'>, parser::range<'a', 'z'>, parser::any<"$", "_">>;
	using valid_name = parser::kh_and
	<
		parser::exact< name_alpha_sym_chars >, // First character should only be A-Z, a-z, $, and _ (dont allow numbers)
		parser::gulp < name_alpha_sym_chars, parser::range<'0', '9'> >, // Same as before but now allow numbers
		parser::exact< parser::any<" "> > // Must have whitespace after
	>;

}