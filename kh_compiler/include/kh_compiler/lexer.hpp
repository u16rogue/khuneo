#pragma once

#include <kh_core/string_literal.hpp>

namespace khuneo::lexer::details
{
	// Classifies a special symbol referring to a reserved keyword
	enum class reserved_kw
	{
		IMPORT,
		FUNCTION,
		LET
	};

	enum class id_type
	{
		RESERVED,
		NAME,
		TYPE
	};

	// Classifies a token
	enum class token_type
	{
		IDENTIFIER, // id_type
		TOKEN	
	};

	constexpr char valid_tokens[] = {
		'+', '-', '*', '/', '%',
		'"', '\'', '\\',
		'(', ')', '[', ']', '{', '}',
		'<', '>', '=', '&', '|', '~', '^', '?',
		'.', ',', ';', ':',
		'#', '@'
	};

	constexpr const char * const valid_keywords[] = {
		"let",
		"fn",
		"import",
		"export",
		"as",
		"for",
		"while",
		"do",
		"if",
		"elif",
		"else"
	};
}

namespace khuneo::lexer
{
	

	


}
