#pragma once

#include <khuneo/compiler/parser.hpp>

namespace khuneo
{
	struct TOKEN_ID { enum
	{
		ROOT_NODE = -1,
		SPACING,
		SYMBOL,
		VARIABLE,
		VAR_TYPE_SET,
		CLOSE,
		CONST_FLAG,
		INTRIN,
		ASSIGNMENT,
		FUNCTION,
		LAMBDA,
		CONST_STRING,
		CONST_NUMBER,
		CONST_FLOATING
	}; };

	template <typename allocator>
	struct lexer
	{
		struct tokens
		{
			using whitespace = parser::any<" ", "\n", "\r", "\t">; // NSE
			using spacing = parser::any<" ", "\t">; // NSE
			using name_alpha_sym_chars = parser::kh_or< parser::range<'A', 'Z'>, parser::range<'a', 'z'>, parser::any<"$", "_"> >; // NSE

			using space_separation = parser::create_token <TOKEN_ID::SPACING, allocator, parser::kh_and
			<
				parser::exact<spacing>, parser::gulp<spacing>
			>>;

			using symbol = parser::create_token<TOKEN_ID::SYMBOL, allocator, parser::kh_and
			<
				parser::exact< name_alpha_sym_chars >,
				parser::gulp < name_alpha_sym_chars, parser::range<'0', '9'> >,
				parser::exact< parser::any<" "> >
			>>;

			using keyword_var = parser::create_token<TOKEN_ID::VARIABLE, allocator, parser::exact<
				parser::any<"var">	
			>>;

			using keyword_const = parser::create_token<TOKEN_ID::CONST_FLAG, allocator, parser::exact
			<
				parser::any<"const">
			>>;

			using var_type_set = parser::create_token<TOKEN_ID::VAR_TYPE_SET, allocator, parser::exact
			<
				parser::any<":">
			>>;

			using close = parser::create_token<TOKEN_ID::CLOSE, allocator, parser::exact
			<
				parser::any<";">
			>>;
		};

		using var_decl = parser::generate_parser
		<
			tokens::keyword_var,
			tokens::space_separation,
			tokens::symbol,
			parser::gulp< tokens::spacing >,
			parser::conditional< parser::any<":">, tokens::var_type_set, parser::gulp< tokens::spacing >, tokens::symbol, parser::gulp< tokens::spacing > >,
			//parser::conditional< parser::any<"=">,  >,
			tokens::close
		>;
	};
}