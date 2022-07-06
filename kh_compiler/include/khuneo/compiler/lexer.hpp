#pragma once

#include <khuneo/core/utf8.hpp>
#include <khuneo/core/metapp.hpp>
#include <khuneo/core/contiguous_list.hpp>

// TODO: assign enum class classification for tokens incase we decide to use utf8 characters for tokens
// TODO: lookup table for RW/toks size

namespace khuneo::compiler::lexer
{
	template <typename lexer_impl> struct msg_callback_info;
}

namespace khuneo::compiler::lexer::details
{
	// Classifies a token
	enum class token_type : khuneo::u8
	{
		UNOCCUPIED = 0,
		UNDEFINED  = UNOCCUPIED,
		SYMBOL,
		KEYWORD,
		TOKEN,
		STRING,
		SIGNED64,
		UNSIGNED64,
		FLOAT64,
	};

	// TODO: reavaluate this, some aren't needed
	constexpr char valid_tokens[] = {
		'"', '#',                                               // 34  - 35
		'%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', // 37  - 47
		':', ';', '<', '=', '>', '?', '@',                      // 58  - 64
		'[', '\\', ']', '^',                                    // 91  - 94
		'{', '|', '}', '~'                                      // 123 - 126
	};

	constexpr int valid_tokens_count = sizeof(valid_tokens) / sizeof(valid_tokens[0]);

	constexpr auto is_valid_token(const char * s) -> bool
	{
		char c = s[0];
		constexpr auto inbetween = [](char c, int low, int high) { return c >= low && c <= high; };
		return c == '"' || c == '#' || inbetween(c, '%', '/') || inbetween(c, ':', '@') || inbetween(c, '[', '^') || inbetween(c, '{', '~'); 
	}

	// Classifies a special symbol referring to a reserved keyword
	enum class reserved_kw : khuneo::u8
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
		ELSE,
		DEFER,
		_INDEXER,
		_COUNT = _INDEXER - 1
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

	constexpr int valid_keywords_count = sizeof(valid_keywords) / sizeof(valid_keywords[0]);
	static_assert(valid_keywords_count == int(reserved_kw::_COUNT), "khuneo::lexer::details::reserved_kw and khuneo::lexer::details::valid_keywords did not match its count.");

	struct sourceloc_tracking_cont
	{
		khuneo::u32 line;
		khuneo::u32 column;
	};

	struct sourceloc_tabspacing_cont
	{
		khuneo::u8 tab_space_count;
	};

	struct default_lexer_impl
	{
		static constexpr bool enable_sloc_track = true;
		using allocator       = khuneo::details::kh_basic_allocator<>;
		using contiguous_list_impl = khuneo::cont::details::default_contiguous_list_impl;
		// static auto lexer_msg_recv(msg_callback_info<enable_sloc_track> * mi) -> void { };
	};
}

namespace khuneo::compiler::lexer
{
	template <typename lexer_impl>
	struct token_node : public metapp::extend_struct_if<lexer_impl::enable_sloc_track, details::sourceloc_tracking_cont>
	{
		details::token_type  type;

		union
		{
			struct
			{
				khuneo::u32 rsource; // Relative source - offset to the matched token
				khuneo::u32 size;
			} symbol, string;

			char token;
			khuneo::i64 signed64;
			khuneo::u64 unsigned64;
			khuneo::f64 float64;
			details::reserved_kw keyword;

		} value;
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
		WARNING_MASK                = 0x7F,
		W_SOURCE_NULL               = 0x7F & 1, // A null character was met in the source buffer before its end
		W_TOKEN_NODE_REUSE_UNMARKED = 0x7F & 2, // A reused token list has a node that is still mark occupied, when reusing token nodes, all nodes next to current must be marked unoccupied, by ignoring this warning the lexer will force reuse that node which COULD lead to undefined behaviour
		W_INVALID_TOKEN             = 0x7F & 3, // Could not determine token

		FATAL_FLAG     = 0x80,
		F_CORRUPT_UTF8 = 0x80 | 1, // A corrupted utf-8 byte was found in the source buffer
		F_SYNTAX_ERROR = 0x80 | 2, // Invalid syntax
		F_ABORTED      = 0x80 | 3, // Operation aborted due to an error
		F_ALLOC_FAIL   = 0x80 | 4, // Failed to allocate memory
		F_UNKNOWN      = 0x80 | 5, // A fatal error occured but it was not clear as to why. This error could be used by the developer for features in production and is under development.
	};

	constexpr auto is_msg_fatal(msg m) -> bool { return khuneo::u8(m) & khuneo::u8(msg::FATAL_FLAG); };

	template <typename lexer_impl> struct run_info;
	template <typename lexer_impl> struct run_state;

	template <typename lexer_impl>
	struct msg_callback_info
	{
		bool ignore;
		msg  message;

		run_info<lexer_impl>  * info;
		run_state<lexer_impl> * state;
	};

	// -----------------------------------------------------------------

	template <typename lexer_impl>
	struct run_state : public metapp::extend_struct_if<lexer_impl::enable_sloc_track, details::sourceloc_tracking_cont>
	{
		bool abort;
		const char * current;
	};

	template <typename lexer_impl>
	struct run_info : public metapp::extend_struct_if<lexer_impl::enable_sloc_track, details::sourceloc_tabspacing_cont>
	{
		const char * start;
		const char * end;
		cont::contiguous_list<token_node<lexer_impl>, typename lexer_impl::contiguous_list_impl> tokens;
	};

	template <typename impl>
	auto run(run_info<impl> * i, run_state<impl> * s = nullptr) -> bool
	{
		constexpr bool sloc_tracking = impl::enable_sloc_track; 
		using allocator              = typename impl::allocator; // metapp::type_if<requires { impl::allocator::alloc(0); }, impl::allocator, details::default_lexer_impl::allocator>::type;
		using state_t                = run_state<impl>;
		using token_node_t           = token_node<impl>; 

		/*
		* Generates a lexer message
		* Returns true if treated as fatal and returns false if it should be ignored 
		*/
		constexpr auto send_msg = [&](msg m) -> bool 
		{
			if constexpr (requires { impl::lexer_msg_recv(0); })
			{
				msg_callback_info<impl> mi;
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
				return true;
		};

		/*
		*  Extends the current token_node list by linking it to the current and replacing current with it
		*  ! This will already send an alloc fail exception, immediately abort upon fail, no need to send your own exception
		*  ! Automatically initializes line, column field. type and value should be provided by the caller
		*/
		auto extend_tail = [&]() -> token_node_t *
		{
			token_node_t * n = i->tokens.append();
			if (!n)
			{
				send_msg(msg::F_ALLOC_FAIL);
				return nullptr;
			}

			n->value      = { 0 };

			if constexpr (sloc_tracking)
			{
				n->line   = s->line;
				n->column = s->column;
			}

			return n;
		};

		auto is_end_v = [&](const char * p) -> bool { return p >= i->end; };
		auto is_end = [&](int offset = 0) -> bool { return s->current + offset >= i->end; };

		// Processes wording characters such as NULL, \r, \n, and \t
		auto process_sloc_char = [&](char c) -> bool
		{
			switch (c)
			{
				case '\0':
				{
					if (send_msg(msg::W_SOURCE_NULL))
						return false;
					break;
				}
				case ' ':
				{
					if constexpr (sloc_tracking)
						++s->column;
					break;
				}
				case '\r':
				{
					if constexpr (sloc_tracking)
						s->column = 0;
					break;
				}
				case '\n':
				{
					if constexpr (sloc_tracking)
					{
						s->column = 0;
						++s->line;
					}
					break;
				}
				case '\t':
				{
					if constexpr (sloc_tracking)
						s->column += i->tab_space_count;
					break;
				}
				default:
					return false;
			}

			return true;
		};

		// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 

		// Internal state if no state is provided (for recursion)
		state_t _s;
		if (!s)
		{
			s = &_s;
			s->current       = i->start;
			s->abort         = false;

			if constexpr (sloc_tracking)
			{
				s->column = 0;
				s->line   = 0;
			}
		}

		while (!is_end() && !s->abort) // LOOP A
		{
			int csz = khuneo::utf8::size(s->current);

			if (csz == 0) // Corrupted byte (no utf8 match) 
			{
				send_msg(msg::F_CORRUPT_UTF8);
				break; // LOOP A
			}

			char cc = *s->current; // current character

			// Match hex
			static constexpr auto is_numeric = [](char c) constexpr -> bool { return c >= '0' && c <= '9'; };
			static constexpr auto is_hex = [](char c) constexpr -> bool { return is_numeric(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'); };
			if (cc == '0' && !is_end(2) && (s->current[1] == 'x' || s->current[1] == 'X') && is_hex(s->current[2]))
			{
				static constexpr auto char_to_hex = [](char c) constexpr -> int
				{
					if (c >= '0' && c <= '9')
						return c - '0';
					else if (c >= 'A' && c <= 'F')
						return c - 'A' + 0xA;
					else if (c >= 'a' && c<= 'f')
						return c - 'a' + 0xa;

					return -1;
				};

				s->current += 2;
				if constexpr (sloc_tracking)
					s->column += 2 + 1 /* the one is for the assignment so we dont have to do it later */;

				token_node_t * t = extend_tail();
				if (!t)
					break; // LOOP A

				t->value.unsigned64 = char_to_hex(*s->current);
				t->type = details::token_type::UNSIGNED64;

				++s->current;	

				while (!is_end() && is_hex(*s->current))
				{
					t->value.unsigned64 *= 16;
					t->value.unsigned64 += char_to_hex(*s->current);
					if constexpr (sloc_tracking)
						++s->column;
					++s->current;
				}

				continue; // LOOP A
			}

			// Match numbers
			if (bool is_negative = cc == '-'; is_numeric(cc) || is_negative && !is_end(1) && is_numeric(s->current[1]))
			{
				static constexpr auto char_to_num = [](char c) constexpr -> int { return c - '0'; };
				token_node_t * t = extend_tail();
				if (!t)
					break; // LOOP A
				t->type = details::token_type::SIGNED64;

				if (is_negative)
					++s->current;

				t->value.signed64 = char_to_num(*s->current);
				++s->current;

				if constexpr (sloc_tracking)
					s->column = is_negative ? 2 : 1;

				khuneo::f64 fdec = 1.0;
				bool stop = is_end();
				while(!stop && is_numeric(*s->current))
				{
					t->value.signed64 *= 10;
					t->value.signed64 += char_to_num(*s->current);

					if (t->type == details::token_type::FLOAT64)
						fdec *= 0.1;

					++s->current;
					if constexpr (sloc_tracking)
						++s->column;

					if (stop = is_end(); !stop && *s->current == '.')
					{
						t->type = details::token_type::FLOAT64;
						++s->current;
						stop = is_end();
					}
				} 

				if (t->type == details::token_type::FLOAT64)
				{
					t->value.float64 = static_cast<khuneo::f64>(t->value.signed64);
					t->value.float64 *= fdec;
				}

				if (is_negative)
				{
					if (t->type == details::token_type::SIGNED64)
						t->value.signed64 *= -1;
					else if (t->type == details::token_type::FLOAT64)
						t->value.float64 *= -1.0;
				}

				continue; // LOOP A
			}

			// Match strings	
			bool has_matched = false;
			constexpr const char strc[] = { '\'', '"', '`' };
			for (char c : strc) // LOOP B
			{
				if (cc != c)
					continue;

				++s->current;
				if constexpr (sloc_tracking)
					++s->column;

				token_node_t * t = extend_tail();
				if (!t)
				{
					has_matched = false;
					break; // LOOP B
				}

				t->type = details::token_type::STRING;
				t->value.string.rsource = s->current - i->start;

				while (!is_end() && *s->current != c)
				{
					int csz = utf8::size(*s->current);
					if (!csz)
						continue;
					s->current += csz;
					if constexpr (sloc_tracking)
						++s->column;
				}

				t->value.string.size = (s->current - i->start) - t->value.string.rsource;
				++s->current; // skip the final "
				if constexpr (sloc_tracking)
					++s->column;

				has_matched = true;
				break; // LOOP B
			}

			if (s->abort)
				break; // LOOP A

			// no need to run check if its a utf8 character because all process_sloc_char matches are ascii
			if (csz == 1 && process_sloc_char(cc)) 
			{
				++s->current;
				continue; // LOOP A
			}

			// Process if possibly a comment
			if (csz == 1 && cc == '/') 
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
					while (!s->abort && !is_end(1) && *s->current != '*' && *(s->current + 1) != '/')
					{
						if (process_sloc_char(*s->current))
							++s->current;
						else
						{
							int mlc_csz = khuneo::utf8::size(s->current);
							if constexpr (sloc_tracking)
								++s->column;
							s->current += mlc_csz; 
						}
					}

					if (*s->current == '*' && *(s->current + 1) == '/')
					{
						s->current += 2;
						if constexpr (sloc_tracking)
							s->column += 2;
					}

					continue; // LOOP A
				}
			}

			// TODO: maybe support utf8 tokens
			// Match token
			if (details::is_valid_token(s->current))
			{
				token_node_t * t = extend_tail();
				if (!t)
					break; // LOOP A

				t->type = details::token_type::TOKEN;
				t->value.token = cc;

				s->current += csz;
				if constexpr (sloc_tracking)
					++s->column;
				continue; // LOOP A
			}

			bool matched = false;
			// Match keyword
			for (int ikw = 0; ikw < details::valid_keywords_count; ++ikw) // LOOP B
			{
				const char * c  = s->current;
				const char * kw = details::valid_keywords[ikw];

				while (*kw && *c) // LOOP C
				{
					if (*kw != *c || is_end_v(c))
						break; // LOOP C

					++kw;
					++c;

					// Match!
					if (*kw == '\0')
					{
						token_node_t * t = extend_tail();
						if (!t)
							break; // LOOP C

						t->type = details::token_type::KEYWORD;
						t->value.keyword = details::reserved_kw(ikw + 1 /*0 is UNDEFINED in enum*/);
						int size = kw - details::valid_keywords[ikw];
						
						if constexpr (sloc_tracking)
							s->column += utf8::length(s->current, s->current + size);
						s->current += size;
						matched = true;
						break; // LOOP C
					}
				}

				if (matched || s->abort)
					break; // LOOP B
			}

			if (s->abort)
				break; // LOOP A

			if (matched)
				continue; // LOOP A (Reserve keyword match)

			// Match symbol
			constexpr auto is_valid_symbolchar = [](char c, bool allow_numeric = true) constexpr -> int
			{
				auto s = khuneo::utf8::size(c);
				if (s == 1)
				{
					return c == '$' || c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (allow_numeric && (c >= '0' && c <= '9'));
				}
				else if (s > 1)
				{
					return s;
				}

				return 0;
			};

			if (int csymlen = is_valid_symbolchar(cc, false); csymlen && !is_end(csymlen))
			{
				token_node_t * t = extend_tail();
				if (!t)
					break; // LOOP A

				t->type = details::token_type::SYMBOL;
				t->value.symbol.rsource = s->current - i->start;
				t->value.symbol.size    = csymlen;

				do
				{
					if constexpr (sloc_tracking)
						++s->column;
					s->current += csymlen;
					csymlen = is_valid_symbolchar(*s->current, true);
					t->value.symbol.size += csymlen;
				} while(!is_end() && csymlen);

				continue; // LOOP A
			}

			// No matches
			if (!send_msg(msg::W_INVALID_TOKEN))
			{
				++s->current;
				if constexpr (sloc_tracking)
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
