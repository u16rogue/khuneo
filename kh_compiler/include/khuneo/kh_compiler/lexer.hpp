#pragma once

namespace khuneo::lexer::details
{
	enum class id_type
	{
		RESERVED,
		NAME,
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

	// Classifies a special symbol referring to a reserved keyword
	enum class reserved_kw
	{
		LET,
		FUNCTION,
		IMPORT,
		EXPORT,
		AS,
		FOR,
		WHILE,
		DO,
		IF,
		ELIF,
		ELSE
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
	struct token_node
	{
		details::token_type type;
		
		union
		{
			int  rsource; // Relative source - offset to the matched token
			char token;
		} value;

		int line;
		int column;
	};

	// -----------------------------------------------------------------

	enum class msg
	{	
	};

	struct run_info;
	struct msg_callback_info
	{
		msg        message;
		run_info * info;
	};
	using msg_callback_fn_t = void(*)(msg_callback_info *); 

	// -----------------------------------------------------------------

	struct run_state
	{
		const char * current;
		int line;
		int column;
	};

	struct run_info 
	{
		const char * start;
		const char * end;

		msg_callback_fn_t msgcb;
		token_node *      tokens;
	};

	template <typename allocator = khuneo::details::kh_allocator<>>
	auto run(run_info * i, run_state * s = nullptr) -> bool
	{
		// Internal state if no state is provided (for recursion)
		run_state _s;
		if (!s)
		{
			s = &_s;
			s->column  = 0;
			s->line    = 0;
			s->current = i->start;
		}

		for (; s->current != i->end; ++s)
		{

		}

	}
}
