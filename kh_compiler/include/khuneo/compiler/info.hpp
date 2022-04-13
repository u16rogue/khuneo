#pragma once

#include <khuneo/defs.hpp>
#include <khuneo/compiler/ast.hpp>

namespace khuneo::impl
{
	struct info;

	using fnparser_t = bool(*)(info *);

	enum class info_stack_type
	{
		UNDEFINED,
		BASIC_STATE,
		NUMBER
	};

	struct info_stack_entry
	{
		info_stack_type type = info_stack_type::UNDEFINED;

		// Holds the information on the current entry
		union
		{
			struct
			{
				const char * start;
				int line;
				int column;
			} basic_state;

			int number;
		};
	};

	struct info
	{
		// Should be considered const and not be modified
		struct
		{
			const char *   start;
			const char *   end;
			ast::node *    root_node;
			int            tab_space { 4 };
			kh_allocator_t allocator; // WARNING: ALLOCATOR MUST ALLOCATE A NULLED BUFFER!
			fnparser_t     parser;
		} ctx;


		// Tracks the current state stack
		struct
		{
			const char * source; // pointer to the current section of the source buffer
			ast::node *  node;   // Pointer to the current AST node
			int line;
			int column;
		} state;

		// Used for simple tracking by values
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

		auto check_overflow(const char * c) -> bool; // Bound checking
		auto check_current_overflow(const int & diff) -> bool; // Bound checking

	private:
		int              stack_counter = 0;
		info_stack_entry stack[16]     = {};
	public:
		auto top() -> info_stack_entry &;
		auto push(info_stack_type type, void * extra_data = nullptr) -> bool;
		auto pop() -> bool;
	};
}