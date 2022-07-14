#pragma once

#include <khuneo/core/utf8.hpp>
#include <khuneo/core/metapp.hpp>
#include <khuneo/core/contiguous_list.hpp>

// TODO: maybe add option to completely ignore warnings at compile (c++) level

namespace khuneo::compiler::lexer
{
	namespace details
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
	}

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

	template <typename lexer_impl> struct run_info;
	template <typename lexer_impl> struct msg_callback_info;
	template <typename lexer_impl> struct token_node;
}

namespace khuneo::compiler::lexer::details
{
	// Classifies a token
	enum class token_type : khuneo::u8
	{
		UNOCCUPIED = 0,
		UNDEFINED = UNOCCUPIED,
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

	/*
	* Generates a lexer message
	* Returns true if treated as fatal and returns false if it should be ignored
	* tldr: true = abort
	*/
	template <typename lexer_impl>
	constexpr auto send_msg(run_info<lexer_impl> * i, msg m) -> bool
	{
		if constexpr (requires { lexer_impl::lexer_msg_recv(0); })
		{
			msg_callback_info<lexer_impl> mi;
			mi.info = i;
			mi.ignore = false;
			mi.message = m;

			lexer_impl::lexer_msg_recv(&mi);

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
	}

	template <typename lexer_impl>
	constexpr auto is_end(run_info<lexer_impl> * i, const char * p) -> bool { return p >= i->end; };

	template <typename lexer_impl>
	constexpr auto is_end(run_info<lexer_impl> * i, int offset = 0) -> bool { return i->current + offset >= i->end; };

	/*
	*  Extends the current token_node list by linking it to the current and replacing current with it
	*  ! This will already send an alloc fail exception, immediately abort upon fail, no need to send your own exception
	*  ! Automatically initializes line, column field. type and value should be provided by the caller
	*/
	template <typename lexer_impl>
	constexpr auto extend_tail(run_info<lexer_impl> * i) -> token_node<lexer_impl> *
	{
		token_node<lexer_impl> * n = i->tokens.append();
		if (!n)
		{
			details::send_msg(i, msg::F_ALLOC_FAIL);
			return nullptr;
		}

		n->value = { 0 };

		if constexpr (lexer_impl::enable_sloc_track)
		{
			n->line = i->line;
			n->column = i->column;
		}

		return n;
	};

	enum class iresp // internal response 
	{
		OK,   // Match was found and has been processed
		PASS, // Match not found, let others try and match
		ABORT // Completely shutdown the lexer
	};

	// Processes wording characters such as NULL, \r, \n, and \t
	template <typename lexer_impl>
	constexpr auto process_sloc_char(run_info<lexer_impl> * i, char c) -> iresp
	{

		if (c == '\0')
		{
			if (details::send_msg(i, msg::W_SOURCE_NULL))
				return iresp::ABORT;
			return iresp::PASS;
		}

		switch (c)
		{
			case ' ':
			{
				if constexpr (lexer_impl::enable_sloc_track)
					++i->column;
				break;
			}
			case '\r':
			{
				if constexpr (lexer_impl::enable_sloc_track)
					i->column = 1;
				break;
			}
			case '\n':
			{
				if constexpr (lexer_impl::enable_sloc_track)
				{
					i->column = 1;
					++i->line;
				}
				break;
			}
			case '\t':
			{
				if constexpr (lexer_impl::enable_sloc_track)
					i->column += i->tab_space_count;
				break;
			}
			default:
				return iresp::PASS;
		}

		++i->current;
		return iresp::OK;
	}

	template <typename lexer_impl>
	constexpr auto match_hex(run_info<lexer_impl> * i) -> iresp
	{
		khuneo::u8 hex_value = 0;
		if (i->current[0] != '0' || details::is_end(i, 2) || !metapp::match_any_a_to_b(i->current[1], 'x', 'X') || !utf8::c_is_hex(i->current[2], &hex_value))
			return iresp::PASS;

		i->current += 2;
		if constexpr (lexer_impl::enable_sloc_track)
			i->column += 2 + 1 /* the one is for the assignment so we dont have to do it later */;

		token_node<lexer_impl> * t = details::extend_tail(i);
		if (!t)
			return iresp::ABORT;

		t->value.unsignedn = hex_value;
		t->type = details::token_type::UNSIGNED;

		++i->current;

		while (!details::is_end(i) && utf8::c_is_hex(i->current[0], &hex_value))
		{
			t->value.unsignedn *= 16;
			t->value.unsignedn += hex_value;
			if constexpr (lexer_impl::enable_sloc_track)
				++i->column;
			++i->current;
		}

		return iresp::OK;
	}

	template <typename lexer_impl>
	constexpr auto match_number(run_info<lexer_impl> * i) -> iresp
	{
		khuneo::u8 numval = 0;
		bool is_negative = i->current[0] == '-';
		if (!utf8::c_is_numeric(i->current[0], &numval) && (!is_negative || details::is_end(i, 1) || !utf8::c_is_numeric(i->current[1], &numval)) )
			return iresp::PASS;

		token_node<lexer_impl> * t = details::extend_tail(i);
		if (!t)
			return iresp::ABORT;

		t->type = details::token_type::SIGNED;

		if (is_negative)
			++i->current;

		t->value.signedn = numval;
		++i->current;

		if constexpr (lexer_impl::enable_sloc_track)
			i->column = is_negative ? 2 : 1;

		typename lexer_impl::float_tok_t fdec = lexer_impl::float_tok_pos;
		bool stop = details::is_end(i);
		while(!stop && utf8::c_is_numeric(i->current[0], &numval)) // LOOP B
		{
			t->value.signedn *= 10;
			t->value.signedn += numval;

			if (t->type == details::token_type::FLOAT)
				fdec *= lexer_impl::float_tok_dec;

			++i->current;
			if constexpr (lexer_impl::enable_sloc_track)
				++i->column;

			if (stop = details::is_end(i); !stop && i->current[0] == '.')
			{
				if (t->type == details::token_type::FLOAT)
				{
					details::send_msg(i, msg::F_SYNTAX_ERROR);
					return iresp::ABORT;
				}

				t->type = details::token_type::FLOAT;
				++i->current;
				stop = details::is_end(i);
			}
		} 

		if (t->type == details::token_type::FLOAT)
		{
			t->value.floatn = static_cast<typename lexer_impl::float_tok_t>(t->value.signedn);
			t->value.floatn *= fdec;
		}

		if (is_negative)
		{
			if (t->type == details::token_type::SIGNED)
				t->value.signedn *= lexer_impl::signed_tok_neg;
			else if (t->type == details::token_type::FLOAT)
				t->value.floatn *= lexer_impl::float_tok_neg;
		}

		return iresp::OK;	
	}

	template <typename lexer_impl>
	constexpr auto match_string(run_info<lexer_impl> * i) -> iresp
	{
		constexpr const char strc[] = { '\'', '"', '`' };
		for (char c : strc)
		{
			if (i->current[0] != c)
				continue;

			++i->current;
			if constexpr (lexer_impl::enable_sloc_track)
				++i->column;

			token_node<lexer_impl> * t = details::extend_tail(i);
			if (!t)
				return iresp::ABORT;

			t->type = details::token_type::STRING;
			t->value.string.rsource = i->current - i->start;

			while (!details::is_end(i) && i->current[0] != c)
			{
				int csz = utf8::csize(*i->current);
				if (!csz)
					continue;
				i->current += csz;
				if constexpr (lexer_impl::enable_sloc_track)
					++i->column;
			}

			t->value.string.size = (i->current - i->start) - t->value.string.rsource;
			++i->current; // skip the final "
			if constexpr (lexer_impl::enable_sloc_track)
				++i->column;

			return iresp::OK;
		}

		return iresp::PASS;
	}

	template <typename lexer_impl>
	constexpr auto match_sloc(run_info<lexer_impl> * i) -> iresp
	{
		return process_sloc_char(i, i->current[0]);
	}

	template <typename lexer_impl>
	constexpr auto match_comment(run_info<lexer_impl> * i) -> iresp
	{
		// Process if possibly a comment
		if (i->current[0] != '/') 
			return iresp::PASS;

		if (details::is_end(i, 1))
		{
			details::send_msg(i, msg::F_SYNTAX_ERROR);
			return iresp::ABORT;
		}

		char nc = i->current[1];
		if (nc == '/') // single line comment
		{
			i->current += 2;
			while (!details::is_end(i) && i->current[0] != '\n')
				++i->current;
			return iresp::PASS;
		}
		else if (nc == '*') // multi line comment
		{
			i->current += 2;
			while (!i->abort && !details::is_end(i, 1) && *i->current != '*' && *(i->current + 1) != '/')
			{
				auto res = details::process_sloc_char(i, i->current[0]);
				if (res == details::iresp::OK)
					++i->current;
				else if (res == details::iresp::ABORT)
					return iresp::ABORT;
				else
				{
					int mlc_csz = khuneo::utf8::csize(i->current[0]);
					if constexpr (lexer_impl::enable_sloc_track)
						++i->column;
					i->current += mlc_csz; 
				}
			}

			if (i->current[0] == '*' && i->current[1] == '/')
			{
				i->current += 2;
				if constexpr (lexer_impl::enable_sloc_track)
					i->column += 2;
			}

			return iresp::OK;
		}

		return iresp::PASS;
	}

	template <typename lexer_impl>
	constexpr auto match_token(run_info<lexer_impl> * i) -> iresp
	{
		for (char tok : details::tokens) // LOOP B
		{
			if (tok != i->current[0])
				continue;

			token_node<lexer_impl> * t = details::extend_tail(i);
			if (!t)
				return iresp::ABORT;

			t->type = details::token_type::TOKEN;
			t->value.token = i->current[0];

			i->current += utf8::csize(t->value.token);
			if constexpr (lexer_impl::enable_sloc_track)
				++i->column;
			return iresp::OK;
		}

		return iresp::PASS;
	}

	template <typename lexer_impl>
	constexpr auto match_keyword(run_info<lexer_impl> * i) -> iresp
	{
		for (int ikw = 0; ikw < metapp::array_size(keywords); ++ikw)
		{
			const char * c  = i->current;
			const char * kw = keywords[ikw];

			while (*kw && *c) // LOOP C
			{
				if (*kw != *c || details::is_end(i, c))
					break; // LOOP C

				++kw;
				++c;

				// Match!
				if (*kw == '\0' && (*c == ' ' || *c == '\t' || *c == '\r' || *c == '\n'))
				{
					token_node<lexer_impl> * t = details::extend_tail(i);
					if (!t)
						return iresp::ABORT;

					t->type = details::token_type::KEYWORD;
					t->value.keyword = details::keyword(ikw + 1 /*0 is UNDEFINED in enum*/);
					int size = kw - details::keywords[ikw];
					
					if constexpr (lexer_impl::enable_sloc_track)
						i->column += utf8::slength(i->current, i->current + size);
					i->current += size;
					return iresp::OK;
				}
			}
		}

		return iresp::PASS;
	}

	template <typename lexer_impl>
	constexpr auto match_symbol(run_info<lexer_impl> * i) -> iresp
	{
		constexpr auto is_valid_symbolchar = [](char c, bool allow_numeric = true) constexpr -> int
		{
			auto s = khuneo::utf8::csize(c);
			if (s == 1)
			{
				return c == '$' || c == '_' || utf8::c_is_alpha(c) || (allow_numeric && utf8::c_is_numeric(c));
			}
			else if (s > 1)
			{
				return s;
			}

			return 0;
		};

		if (int csymlen = is_valid_symbolchar(i->current[0], false); csymlen && !details::is_end(i, csymlen))
		{
			token_node<lexer_impl> * t = details::extend_tail(i);
			if (!t)
				return iresp::ABORT;

			t->type = details::token_type::SYMBOL;
			t->value.symbol.rsource = i->current - i->start;
			t->value.symbol.size    = csymlen;

			do
			{
				if constexpr (lexer_impl::enable_sloc_track)
					++i->column;
				i->current += csymlen;
				csymlen = is_valid_symbolchar(i->current[0], true);
				t->value.symbol.size += csymlen;
			} while(!details::is_end(i) && csymlen);

			return iresp::OK;
		}

		return iresp::PASS;
	}

	template <typename impl>
	constexpr iresp(*matches[])(run_info<impl>*) =
	{
		match_hex<impl>,
		match_number<impl>,
		match_string<impl>,
		match_sloc<impl>,	
		match_comment<impl>,
		match_token<impl>,
		match_keyword<impl>,
		match_symbol<impl>
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

	constexpr auto is_msg_fatal(msg m) -> bool { return khuneo::u8(m) & khuneo::u8(msg::FATAL_FLAG); };

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

		while (!details::is_end(i) && !i->abort) // LOOP A
		{
			if (utf8::csize(*i->current) == 0) // Corrupted byte (no utf8 match) 
			{
				details::send_msg(i, msg::F_CORRUPT_UTF8);
				break; // LOOP A
			}

			details::iresp response = details::iresp::ABORT;
			for (auto matcher : details::matches<impl>) // LOOP B
			{
				response = matcher(i);
				if (response == details::iresp::PASS)
					continue; // LOOP B

				break; // LOOP B
			}

			if (response == details::iresp::ABORT)
				break; // LOOP A
			else if (response == details::iresp::PASS) // No match
			{
				if (details::send_msg(i, msg::W_INVALID_TOKEN))
					break; // LOOP A
				++i->current;
				if constexpr (sloc_tracking)
					++i->column;
			}
		}

		if (i->abort)
		{
			details::send_msg(i, msg::F_ABORTED);
			return false;
		}

		if (i->current != i->end && details::send_msg(i, msg::W_PAST_END))
			return false;	

		return true;
	}
}
