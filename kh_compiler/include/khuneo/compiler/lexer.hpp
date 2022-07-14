#pragma once

#include <khuneo/core/utf8.hpp>
#include <khuneo/core/metapp.hpp>
#include <khuneo/core/contiguous_list.hpp>

// TODO: maybe add option to completely ignore warnings at compile (c++) level

namespace khuneo::compiler::lexer::details
{
	/*
	*  This is the default implementation used by the lexer, you can use this or derive from it since C++
	*  allows overloading you can simply inherit this structure and override specific parts if you don't
	*  want to implement it from scratch
	*  e.g. Enabling receiving messages from the lexer
	*  struct my_impl : public khuneo::compiler::lexer::details::default_lexer_impl
	*  {
	*		static auto lexer_msg_recv(khuneo::compiler::lexer::msg_callback_info<my_impl> * mi) -> void
	*		{
	*		}
	*  }
	*  ...
	*  khuneo::compiler::lexer::run<my_impl>(...);
	*/
	struct default_lexer_impl
	{
		static constexpr bool enable_sloc_track = true;                                                // Toggles if the lexer should keep track of lines and column of each token
		using allocator                         = khuneo::details::kh_basic_allocator<>;               // Defines the allocator to be used
		using contiguous_list_impl              = khuneo::cont::details::default_contiguous_list_impl; // Defines the implementation used by the contiguous list container
		// static auto lexer_msg_recv(msg_callback_info<enable_sloc_track> * mi) -> void { };          // Lexer message receiver callback

		using signed_tok_t   = khuneo::i64; // Type of the signed token
		using unsigned_tok_t = khuneo::u64; // Type of the unsigned token
		using float_tok_t    = khuneo::f64; // Type of the floating point token

		static constexpr float_tok_t  float_tok_pos  =  1.0; // Positive 1 value for floating point type
		static constexpr float_tok_t  float_tok_neg  = -1.0; // Negation value for floating point type
		static constexpr float_tok_t  float_tok_dec  =  0.1; // Decimal multiplier value for floating point type
		static constexpr signed_tok_t signed_tok_neg = -1; // Negation value for signed type
	};

	// Classifies a token
	enum class token_type : khuneo::u8
	{
		UNOCCUPIED = 0,
		UNDEFINED  = UNOCCUPIED,
		SYMBOL,
		KEYWORD,
		TOKEN,
		STRING,
		SIGNED,
		UNSIGNED,
		FLOAT,
	};

	// ---------------------------------------------------------------------------------------------------- 

	constexpr char tokens[] = {
		/*'"',*/ '#',                                               // 34  - 35
		'%', '&', /*'\'',*/ '(', ')', '*', '+', ',', '-', '.', '/', // 37  - 47
		':', ';', '<', '=', '>', '?', '@',                          // 58  - 64
		'[', '\\', ']', '^',                                        // 91  - 94
		'{', '|', '}', '~'                                          // 123 - 126
	};

	/*
	constexpr auto is_valid_token(const char * s) -> bool
	{
		char c = s[0];
		constexpr auto inbetween = [](char c, int low, int high) constexpr -> bool { return c >= low && c <= high; };
		return c == '"' || c == '#' || inbetween(c, '%', '/') || inbetween(c, ':', '@') || inbetween(c, '[', '^') || inbetween(c, '{', '~'); 
	}
	*/

	// ---------------------------------------------------------------------------------------------------- 

	// Classifies a special symbol referring to a reserved keyword
	enum class keyword : khuneo::u8
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

	constexpr const char * const keywords[] = {
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
		"defer" // thank you zig
	};

	// ---------------------------------------------------------------------------------------------------- 

	struct sourceloc_tracking_cont
	{
		khuneo::u32 line;
		khuneo::u32 column;
	};

	struct sourceloc_tabspacing_cont
	{
		khuneo::u8 tab_space_count;
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
			typename lexer_impl::signed_tok_t   signedn;
			typename lexer_impl::unsigned_tok_t unsignedn;
			typename lexer_impl::float_tok_t    floatn;
			details::keyword keyword;

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
		W_PAST_END                  = 0x7F & 4, // Lexer parsing has ended but the cursor went past the end, this could be a sign of a corrupted buffer, corrupted UTF-8, or a misconfigured contex. It is recommended to NOT ignore this warning

		FATAL_FLAG     = 0x80,
		F_CORRUPT_UTF8 = 0x80 | 1, // A corrupted utf-8 byte was found in the source buffer
		F_SYNTAX_ERROR = 0x80 | 2, // Invalid syntax
		F_ABORTED      = 0x80 | 3, // Operation aborted due to an error
		F_ALLOC_FAIL   = 0x80 | 4, // Failed to allocate memory
		F_UNKNOWN      = 0x80 | 5, // A fatal error occured but it was not clear as to why. This error could be used by the developer for features in production and is under development.
	};

	constexpr auto is_msg_fatal(msg m) -> bool { return khuneo::u8(m) & khuneo::u8(msg::FATAL_FLAG); };

	template <typename lexer_impl> struct run_info;

	template <typename lexer_impl>
	struct msg_callback_info
	{
		bool ignore;
		msg  message;

		run_info<lexer_impl>  * info;
	};

	// -----------------------------------------------------------------

	template <typename lexer_impl>
	struct run_info : public metapp::extend_struct_if<lexer_impl::enable_sloc_track, details::sourceloc_tabspacing_cont>, metapp::extend_struct_if<lexer_impl::enable_sloc_track, details::sourceloc_tracking_cont>
	{
		const char * start;
		const char * end;
		bool abort;
		const char * current;
		cont::contiguous_list<token_node<lexer_impl>, typename lexer_impl::contiguous_list_impl> tokens;
	};

	template <typename impl>
	auto run(run_info<impl> * i) -> bool
	{
		constexpr bool sloc_tracking = impl::enable_sloc_track; 
		using allocator              = typename impl::allocator; // metapp::type_if<requires { impl::allocator::alloc(0); }, impl::allocator, details::default_lexer_impl::allocator>::type;
		using token_node_t           = token_node<impl>; 

		/*
		* Generates a lexer message
		* Returns true if treated as fatal and returns false if it should be ignored 
		*/
		auto send_msg = [&](msg m) -> bool 
		{
			if constexpr (requires { impl::lexer_msg_recv(0); })
			{
				msg_callback_info<impl> mi;
				mi.info = i;
				mi.ignore = false;
				mi.message = m;

				impl::lexer_msg_recv(&mi);

				if (is_msg_fatal(mi.message))
				{
					i->abort = true;
					return true;
				}

				i->abort = !mi.ignore;
				return i->abort;
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

			n->value = { 0 };

			if constexpr (sloc_tracking)
			{
				n->line   = i->line;
				n->column = i->column;
			}

			return n;
		};

		auto is_end_v = [&](const char * p) -> bool { return p >= i->end; };
		auto is_end   = [&](int offset = 0) -> bool { return i->current + offset >= i->end; };

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
						++i->column;
					break;
				}
				case '\r':
				{
					if constexpr (sloc_tracking)
						i->column = 0;
					break;
				}
				case '\n':
				{
					if constexpr (sloc_tracking)
					{
						i->column = 0;
						++i->line;
					}
					break;
				}
				case '\t':
				{
					if constexpr (sloc_tracking)
						i->column += i->tab_space_count;
					break;
				}
				default:
					return false;
			}

			return true;
		};

		// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 

		while (!is_end() && !i->abort) // LOOP A
		{
			int csz = khuneo::utf8::csize(*i->current);

			if (csz == 0) // Corrupted byte (no utf8 match) 
			{
				send_msg(msg::F_CORRUPT_UTF8);
				break; // LOOP A
			}

			char cc = *i->current; // current character

			// Match hex
			static constexpr auto is_numeric = [](char c) constexpr -> bool { return c >= '0' && c <= '9'; };
			static constexpr auto is_hex = [](char c) constexpr -> bool { return is_numeric(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'); };
			if (cc == '0' && !is_end(2) && (i->current[1] == 'x' || i->current[1] == 'X') && is_hex(i->current[2]))
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

				i->current += 2;
				if constexpr (sloc_tracking)
					i->column += 2 + 1 /* the one is for the assignment so we dont have to do it later */;

				token_node_t * t = extend_tail();
				if (!t)
					break; // LOOP A

				t->value.unsignedn = char_to_hex(*i->current);
				t->type = details::token_type::UNSIGNED;

				++i->current;

				while (!is_end() && is_hex(*i->current))
				{
					t->value.unsignedn *= 16;
					t->value.unsignedn += char_to_hex(*i->current);
					if constexpr (sloc_tracking)
						++i->column;
					++i->current;
				}

				continue; // LOOP A
			}

			// Match numbers
			if (bool is_negative = cc == '-'; is_numeric(cc) || is_negative && !is_end(1) && is_numeric(i->current[1]))
			{
				static constexpr auto char_to_num = [](char c) constexpr -> int { return c - '0'; };
				token_node_t * t = extend_tail();
				if (!t)
					break; // LOOP A
				t->type = details::token_type::SIGNED;

				if (is_negative)
					++i->current;

				t->value.signedn = char_to_num(*i->current);
				++i->current;

				if constexpr (sloc_tracking)
					i->column = is_negative ? 2 : 1;

				typename impl::float_tok_t fdec = impl::float_tok_pos;
				bool stop = is_end();
				while(!stop && is_numeric(*i->current)) // LOOP B
				{
					t->value.signedn *= 10;
					t->value.signedn += char_to_num(*i->current);

					if (t->type == details::token_type::FLOAT)
						fdec *= impl::float_tok_dec;

					++i->current;
					if constexpr (sloc_tracking)
						++i->column;

					if (stop = is_end(); !stop && *i->current == '.')
					{
						if (t->type == details::token_type::FLOAT)
						{
							send_msg(msg::F_SYNTAX_ERROR);
							break; // LOOP B
						}
						t->type = details::token_type::FLOAT;
						++i->current;
						stop = is_end();
					}
				} 

				if (i->abort)
					break; // LOOP A

				if (t->type == details::token_type::FLOAT)
				{
					t->value.floatn = static_cast<typename impl::float_tok_t>(t->value.signedn);
					t->value.floatn *= fdec;
				}

				if (is_negative)
				{
					if (t->type == details::token_type::SIGNED)
						t->value.signedn *= impl::signed_tok_neg;
					else if (t->type == details::token_type::FLOAT)
						t->value.floatn *= impl::float_tok_neg;
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

				++i->current;
				if constexpr (sloc_tracking)
					++i->column;

				token_node_t * t = extend_tail();
				if (!t)
				{
					has_matched = false;
					break; // LOOP B
				}

				t->type = details::token_type::STRING;
				t->value.string.rsource = i->current - i->start;

				while (!is_end() && *i->current != c)
				{
					int csz = utf8::csize(*i->current);
					if (!csz)
						continue;
					i->current += csz;
					if constexpr (sloc_tracking)
						++i->column;
				}

				t->value.string.size = (i->current - i->start) - t->value.string.rsource;
				++i->current; // skip the final "
				if constexpr (sloc_tracking)
					++i->column;

				has_matched = true;
				break; // LOOP B
			}

			if (i->abort)
				break; // LOOP A

			// no need to run check if its a utf8 character because all process_sloc_char matches are ascii
			if (csz == 1 && process_sloc_char(cc)) 
			{
				++i->current;
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

				char nc = *(i->current + 1);
				if (nc == '/') // single line comment
				{
					i->current += 2;
					while (!is_end() && *i->current != '\n')
						++i->current;
					continue; // LOOP A // let process_wordings deal with it since we are now at the \n
				}
				else if (nc == '*') // multi line comment
				{
					i->current += 2;
					while (!i->abort && !is_end(1) && *i->current != '*' && *(i->current + 1) != '/')
					{
						if (process_sloc_char(*i->current))
							++i->current;
						else
						{
							int mlc_csz = khuneo::utf8::csize(*i->current);
							if constexpr (sloc_tracking)
								++i->column;
							i->current += mlc_csz; 
						}
					}

					if (*i->current == '*' && *(i->current + 1) == '/')
					{
						i->current += 2;
						if constexpr (sloc_tracking)
							i->column += 2;
					}

					continue; // LOOP A
				}
			}

			bool matched = false;
			// TODO: maybe support utf8 tokens
			// Match token
			for (char tok : details::tokens) // LOOP B
			{
				if (tok != cc)
					continue;

				token_node_t * t = extend_tail();
				if (!t)
					break; // LOOP B

				t->type = details::token_type::TOKEN;
				t->value.token = cc;

				i->current += csz;
				if constexpr (sloc_tracking)
					++i->column;
				matched = true;
				break; // LOOP B
			}

			if (i->abort)
				break; // LOOP A

			if (matched)
				continue; // LOOP A

			// Match keyword
			for (int ikw = 0; ikw < metapp::array_size(details::keywords); ++ikw) // LOOP B
			{
				const char * c  = i->current;
				const char * kw = details::keywords[ikw];

				while (*kw && *c) // LOOP C
				{
					if (*kw != *c || is_end_v(c))
						break; // LOOP C

					++kw;
					++c;

					// Match!
					if (*kw == '\0' && (*c == ' ' || *c == '\t' || *c == '\r' || *c == '\n'))
					{
						token_node_t * t = extend_tail();
						if (!t)
							break; // LOOP C

						t->type = details::token_type::KEYWORD;
						t->value.keyword = details::keyword(ikw + 1 /*0 is UNDEFINED in enum*/);
						int size = kw - details::keywords[ikw];
						
						if constexpr (sloc_tracking)
							i->column += utf8::slength(i->current, i->current + size);
						i->current += size;
						matched = true;
						break; // LOOP C
					}
				}

				if (matched || i->abort)
					break; // LOOP B
			}

			if (i->abort)
				break; // LOOP A

			if (matched)
				continue; // LOOP A (Reserve keyword match)

			// Match symbol
			static constexpr auto is_valid_symbolchar = [](char c, bool allow_numeric = true) constexpr -> int
			{
				auto s = khuneo::utf8::csize(c);
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
				t->value.symbol.rsource = i->current - i->start;
				t->value.symbol.size    = csymlen;

				do
				{
					if constexpr (sloc_tracking)
						++i->column;
					i->current += csymlen;
					csymlen = is_valid_symbolchar(*i->current, true);
					t->value.symbol.size += csymlen;
				} while(!is_end() && csymlen);

				continue; // LOOP A
			}

			// No matches
			if (!send_msg(msg::W_INVALID_TOKEN))
			{
				++i->current;
				if constexpr (sloc_tracking)
					++i->column;
				continue;
			}

			return false;
		}

		if (i->abort)
		{
			send_msg(msg::F_ABORTED);
			return false;
		}

		if (i->current != i->end && send_msg(msg::W_PAST_END))
			return false;	

		return true;
	}
}
