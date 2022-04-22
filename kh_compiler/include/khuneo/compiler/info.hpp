#pragma once

#include <khuneo/defs.hpp>
#include <khuneo/compiler/ast.hpp>

namespace khuneo
{
	namespace impl { struct info; }
	struct compiler_exception
	{
		impl::info * info_instance;
		const char * message;
		int line;
		int column;
	};
}

namespace khuneo::impl
{
	struct info;

	using fnparser_t = bool(*)(info *);
	using fn_parser_except_t = void(*)(khuneo::compiler_exception *);

	enum class info_stack_type
	{
		// No stack type defined
		UNDEFINED,

		// Stores the <info> basic state, basic in a sense that it keeps
		// simple information that's only needed for its purpose
		BASIC_STATE,

		// Stores a signed integer passed from the extradata parameter of a push
		NUMBER,

		// Stores the current exception handler if a rule set fails the most recent
		// exception pushed into the stack is used.
		EXCEPTION
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

			khuneo::compiler_exception exception;

			int number;
		};
	};

	struct info
	{
		// Should be considered const and not be modified
		struct
		{
			const char *       start;
			const char *       end;
			ast::node *        root_node;
			int                tab_space { 4 };
			kh_allocator_t     allocator; // WARNING: ALLOCATOR MUST ALLOCATE A NULLED BUFFER!
			fn_parser_except_t parser_exception;
			fnparser_t         parser;
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

		auto generate_exception(const char * message) -> void;
		auto h_allocate_node() -> ast::node *;

	private:
		int              stack_counter = 0;
		info_stack_entry stack[16]     = {};
	public:
		auto stack_top() -> info_stack_entry &;
		auto stack_push(info_stack_type type, void * extra_data = nullptr) -> bool;
		auto stack_pop() -> bool;
		auto stack_count() -> int;
		auto stack_indexed(int i) -> info_stack_entry &;
		auto stack_find_recent(info_stack_type type) -> info_stack_entry *;
	};
}