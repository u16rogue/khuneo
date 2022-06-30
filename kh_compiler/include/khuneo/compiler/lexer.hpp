#pragma once

#include <khuneo/core/utf8.hpp>
#include <khuneo/core/metapp.hpp>

namespace khuneo::lexer
{
	template <bool enable_wording_track> struct msg_callback_info;
}

namespace khuneo::lexer::details
{
	enum class id_type
	{
		UNDEFINED,
		RESERVED,
		NAME,
	};

	// Classifies a token
	enum class token_type
	{
		UNDEFINED,
		IDENTIFIER, // id_type
		TOKEN,
		NUMBER,
		STRING,
		FLOAT
	};

	constexpr char valid_tokens[] = {
		'"', '#',                                               // 34  - 35
		'%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', // 37  - 47
		':', ';', '<', '=', '>', '?', '@',                      // 58  - 64
		'[', '\\', ']', '^',                                    // 91  - 94
		'{', '|', '}', '~'                                      // 123 - 126
	};

	constexpr int valid_tokens_count = sizeof(valid_tokens) / sizeof(valid_tokens[0]);

	constexpr auto is_valid_token(char c) -> bool
	{
		constexpr auto inbetween = [](char c, int low, int high) { return c >= low && c <= high; };
		return c == '"' || c == '#' || inbetween(c, '%', '/') || inbetween(c, ':', '@') || inbetween(c, '[', '^') || inbetween(c, '{', '~'); 
	}

	// Classifies a special symbol referring to a reserved keyword
	enum class reserved_kw
	{
		UNDEFINED,
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
		"else",
		"defer"
	};

	struct wording_tracking_cont
	{
		khuneo::u32 line;
		khuneo::u32 column;
	};

	struct wording_tabspacing_cont
	{
		khuneo::u8 tab_space_count;
	};

	struct default_lexer_impl
	{
		static constexpr bool enable_wording_track = true;
		using allocator = khuneo::details::kh_allocator<>;
		static auto lexer_msg_recv(msg_callback_info<enable_wording_track> * mi) -> void { };
	};
}

namespace khuneo::lexer
{
	template <bool enable_wording_track = true>
	struct token_node : public metapp::extend_struct_if<enable_wording_track, details::wording_tracking_cont>
	{
		bool occupied;
		
		union
		{
			khuneo::u32  rsource; // Relative source - offset to the matched token
			char token;
		} value;

		khuneo::u32 rlast;
		khuneo::u32 size;

		details::token_type type;
		token_node * next_token;
	};

	// -----------------------------------------------------------------

	/*
	* Defines the message type.
	*
	* Messages that has its most significant bit
	* set to 1 means its fatal which cannot be ignored
	*/
	enum class msg : khuneo::u8
	{
		W_SOURCE_NULL               = 0x7F & 1, // A null character was met in the source buffer before its end
		W_TOKEN_NODE_REUSE_UNMARKED = 0x7F & 2, // A resused token list has a node that is still mark occupied, when reusing token nodes, all nodes next to current must be marked unoccupied, by ignoring this warning the lexer will force reuse that node which COULD lead to undefined behaviour
		W_INVALID_TOKEN             = 0x7F & 3, // Could not determine token

		FATAL_FLAG     = 0x80,
		F_CORRUPT_UTF8 = 0x80 | 1, // A corrupted utf-8 byte was found in the source buffer
		F_SYNTAX_ERROR = 0x80 | 2, // Invalid syntax
		F_ABORTED      = 0x80 | 3, // Operation aborted due to an error
		F_ALLOC_FAIL   = 0x80 | 4, // Failed to allocate memory
		F_UNKNOWN      = 0x80 | 5, // A fatal error occured but it was not clear as to why. This error could be used by the developer for features in production and is under development.
	};

	constexpr auto is_msg_fatal(msg m) -> bool { return khuneo::u8(m) & khuneo::u8(msg::FATAL_FLAG); };

	template <bool enable_wording_track> struct run_info;
	template <bool enable_wording_track> struct run_state;

	template <bool enable_wording_track>
	struct msg_callback_info
	{
		bool        ignore;
		msg         message;
		run_info<enable_wording_track>  * info;
		run_state<enable_wording_track> * state;
	};

	// -----------------------------------------------------------------

	template <bool enable_wording_track>
	struct run_state : public metapp::extend_struct_if<enable_wording_track, details::wording_tracking_cont>
	{
		bool abort;
		const char * current;
		token_node<enable_wording_track> * current_token;
	};

	template <bool enable_wording_track>
	struct run_info : public metapp::extend_struct_if<enable_wording_track, details::wording_tabspacing_cont>
	{
		const char * start;
		const char * end;
		token_node<enable_wording_track> * tokens;
	};

	template <typename impl = details::default_lexer_impl>
	auto run(run_info<impl::enable_wording_track> * i, run_state<impl::enable_wording_track> * s = nullptr) -> bool
	{
		constexpr bool word_tracking = impl::enable_wording_track; 
		using allocator              = typename impl::allocator; // metapp::type_if<requires { impl::allocator::alloc(0); }, impl::allocator, details::default_lexer_impl::allocator>::type;
		using state_t                = run_state<word_tracking>;
		using token_node_t           = token_node<word_tracking>; 

		// Internal state if no state is provided (for recursion)
		state_t _s;
		if (!s)
		{
			s = &_s;
			s->current       = i->start;
			s->current_token = i->tokens;
			s->abort         = false;

			if constexpr (word_tracking)
			{
				s->column = 0;
				s->line   = 0;
			}
		}

		/*
		* Generates a lexer message
		* Returns true if treated as fatal and returns false if it should be ignored 
		*/
		auto send_msg = [&](msg m) -> bool 
		{
			if constexpr (requires { impl::lexer_msg_recv(0); })
			{
				msg_callback_info<word_tracking> mi;
				mi.info = i;
				mi.ignore = false;
				mi.message = m;
				mi.state = s;

				impl::lexer_msg_recv(&mi);

				if (is_msg_fatal(mi.message))
				{
					s->abort = true;
					return true;
				}

				s->abort = !mi.ignore;
				return s->abort;
			}
			else
				return false;
		};

		/*
		*  Extends the current token_node list by linking it to the current and replacing current with it
		*  ! This will already send an alloc fail exception, immediately abort upon fail, no need to send your own exception
		*  ! Automatically initializes line, column field. type and value should be provided by the caller
		*  ! Will automatically append itself to state, linking it into the list and replacing current with itself
		*  ! If it encounters an unoccupied node instead of allocating a new node it will instead re initialize that node and re use it
		*/
		auto extend_tail = [&]() -> token_node_t *
		{
			token_node_t * n = nullptr;
			if (!s->current_token // If there is no token
			||  s->current_token && s->current_token->occupied && !s->current_token->next_token // If there is a token but its already occupied and there is no next
			) {
				n = allocator::template talloc<token_node_t>();
				if (!n)
				{
					send_msg(msg::F_ALLOC_FAIL);
					return nullptr;
				}
				n->next_token = nullptr;
			}
			else if (s->current_token && !s->current_token->occupied) // If there is a token but its not occupied yet
			{
				n = s->current_token;
			}
			else if (s->current_token && s->current_token->occupied && s->current_token->next_token)
			{
				if (s->current_token->next_token->occupied)
				{
					if (!send_msg(msg::W_TOKEN_NODE_REUSE_UNMARKED))
						return nullptr;
				}

				n = s->current_token->next_token;
			}
			
			if (!n)
			{
				send_msg(msg::F_UNKNOWN); // n was still null for some reason
				return nullptr;
			}

			n->occupied   = true;
			n->type       = details::token_type::UNDEFINED;
			n->value      = { 0 };

			if constexpr (word_tracking)
			{
				n->line       = s->line;
				n->column     = s->column;
			}

			if (!s->current_token)
			{
				s->current_token = n;
				// if (!i->tokens) // should be developer's responsibility :>
				//	i->tokens = n;
			}
			else if (s->current_token && s->current_token != n)
			{
				s->current_token->next_token = n;
				s->current_token = n;
			}

			return n;
		};

		auto is_end = [&](int offset = 0) -> bool { return s->current + offset == i->end; };

		// Processes wording characters such as NULL, \r, \n, and \t
		auto process_wordings = [&](char c) -> bool
		{
			switch (c)
			{
				case '\0':
				{
					if (send_msg(msg::W_SOURCE_NULL))
						return false;
					break;
				}
				case '\r':
				{
					if constexpr (word_tracking)
						s->column = 0;
					break;
				}
				case '\n':
				{
					if constexpr (word_tracking)
					{
						s->column = 0;
						++s->line;
					}
					break;
				}
				case '\t':
				{
					if constexpr (word_tracking)
					{
						s->column += i->tab_space_count;
					}
					break;
				}
				default:
					return false;
			}

			return true;
		};

		while (!is_end() && !s->abort) // LOOP A
		{
			int csz = khuneo::utf8::size(s->current);

			if (csz == 0) // Corrupted byte (no utf8 match) 
			{
				send_msg(msg::F_CORRUPT_UTF8);
				break; // LOOP A
			}

			char cc = *s->current; // current character

			if (csz == 1 && process_wordings(cc)) // no need to run check if its a utf8 character because all process_wordings matches are ascii
			{
				++s->current;
				continue; // LOOP A
			}
			else if (csz == 1 && cc == '/') // Process if possibly a comment
			{
				if (is_end(1))
				{
					send_msg(msg::F_SYNTAX_ERROR);
					break; // LOOP A
				}

				char nc = *(s->current + 1);
				if (nc == '/') // single line comment
				{
					s->current += 2;
					while (!is_end() && *s->current != '\n')
						++s->current;
					continue; // LOOP A // let process_wordings deal with it since we are now at the \n
				}
				else if (nc == '*') // multi line comment
				{
					s->current += 2;
					while (!s->abort && !is_end() && !is_end(1) && *s->current != '*' && *(s->current + 1) != '/')
					{
						if (process_wordings(*s->current))
							++s->current;
						else
						{
							int mlc_csz = khuneo::utf8::size(s->current);
							if constexpr (word_tracking)
								++s->column;
							s->current += mlc_csz; 
						}
					}

					if (*s->current == '*' && *(s->current + 1) == '/')
					{
						s->current += 2;
						if constexpr (word_tracking)
							s->column += 2;
					}

					continue; // LOOP A
				}
			}
			else
			{
				if (details::is_valid_token(cc))
				{
				}
				else
				{
				}
			}

			if (!send_msg(msg::W_INVALID_TOKEN))
			{
				++s->current;
				if constexpr (word_tracking)
					++s->column;
				continue;
			}

			return false;
		}

		if (s->abort)
		{
			send_msg(msg::F_ABORTED);
			return false;
		}

		return true;
	}
}
