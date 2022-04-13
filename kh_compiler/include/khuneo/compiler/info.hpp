#pragma once

#include <khuneo/defs.hpp>
#include <khuneo/compiler/ast.hpp>

namespace khuneo::impl
{
	struct info;

	using fnparser_t = bool(*)(info *);

	struct info
	{
		// Should be considered const and not be modified
		struct
		{
			const char *   start;
			const char *   end;
			ast::node *    root_node;
			int            tab_space { 4 };
			kh_allocator_t allocator;
			fnparser_t     parser;
		} ctx;

		struct
		{
			const char * source; // pointer to the current source buffer
			ast::node *  node;   // Pointer to the current AST node
		} state;

		struct
		{
			int line;
			int column;
		} text;

		union
		{
			struct
			{
				int value;
			} response;

			struct
			{
				int value;
			} request;
		};

		// Used by begin_token and insert_token
		// DOES NOT STACK!
		struct
		{
			const char * start;
			int line;
			int column;
		} token_data;

		auto check_overflow(const char * c) -> bool; // Bound checking
		auto check_current_overflow(const int & diff) -> bool; // Bound checking

		// Pushes the current information to token_data which is used by the tokenizer
		auto push_tokend() -> void;

		// Applies the current information stored to a node
		auto pop_tokend(ast::node * n) -> void;
	};
}