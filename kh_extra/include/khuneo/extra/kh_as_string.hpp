#pragma once

#include <cstdio>

#ifdef KH_EXTRA_COMPILER_AVAILABLE
	#include <khuneo/compiler/lexer.hpp>
#endif

namespace khuneo
{
	// TODO: Make our own snprintf
	template <int count, typename... vargs_t>
	constexpr auto sformat(char (&out)[count], const char * format, vargs_t... vargs) -> int
	{
		return snprintf(out, count, format, vargs...);
	}
}

namespace khuneo::extra
{



	#ifdef KH_EXTRA_COMPILER_AVAILABLE

	constexpr auto msg_string(khuneo::compiler::lexer::msg m) -> const char *
	{
		constexpr const char * msg_map_warning[] = {
			"A null character was met in the source buffer before its end",
			"A reused token list has a node that is still mark occupied, when reusing token nodes, all nodes next to current must be marked unoccupied, by ignoring this warning the lexer will force reuse that node which COULD lead to undefined behaviour",
			"Could not determine token",
			"Lexer parsing has ended but the cursor went past the end, this could be a sign of a corrupted buffer, corrupted UTF-8, or a misconfigured context. It is recommended to NOT ignore this warning"
		};

		constexpr const char * msg_map_fatal[] = {
			"A corrupted UTF-8 byte was found in the source buffer",
			"Invalid syntax",
			"Operation aborted due to an error",
			"Failed to allocate memory",
			"A fatal error occured but it was not clear as to why. This error could be used by the developer for features in production and is under development."
		};

		const char ** msg_source[] = { (const char **)msg_map_warning, (const char **)msg_map_fatal };

		return msg_source[khuneo::compiler::lexer::is_msg_fatal(m)][(khuneo::u8(m) & 0x7F) - 1];
	}

	template <int sz, typename lexer_impl>
	constexpr auto msg_format(char (&out)[sz], const khuneo::compiler::lexer::msg_callback_info<lexer_impl> * m) -> bool
	{
		bool is_fatal = khuneo::compiler::lexer::is_msg_fatal(m->message);
		const char * m_type = is_fatal ? "FATAL" : "WARNING";
		int count = 0;

		if constexpr (lexer_impl::enable_sloc_track)
		{
			count = khuneo::sformat(out, "[%s | %d,%d] %s",
					 m_type,
					 m->info->line,
					 m->info->column,
					 msg_string(m->message) 
			);
		}
		else
		{
			count = khuneo::sformat(out, "[%s] %s",
					 m_type,
					 msg_string(m->message) 
			);
		}

		return count > 0 && count < sz;
	}

	template <typename lexer_impl>
	constexpr auto token_type_to_string(const khuneo::compiler::lexer::token_node<lexer_impl> * t) -> const char *
	{
		constexpr const char * ttype_map[] = {
			"UNOCCUPIED",
			"SYMBOL",
			"KEYWORD",
			"TOKEN",
			"STRING",
			"SIGNED",
			"UNSIGNED",
			"FLOAT",
		};

		return ttype_map[int(t->type)]; 
	}

	template <int sz, typename lexer_impl>
	constexpr auto token_value_to_string(char (&out)[sz], const khuneo::compiler::lexer::token_node<lexer_impl> * t, const khuneo::compiler::lexer::run_info<lexer_impl> * info = nullptr) -> bool
	{
		namespace lx = khuneo::compiler::lexer::details;
		int count = 0;
		switch (t->type)
		{
			case lx::token_type::KEYWORD:
			{
				count = khuneo::sformat(out, "%s", lx::keywords[int(t->value.keyword) - 1]);
				break;
			}
			case lx::token_type::FLOAT:
			{
				count = khuneo::sformat(out, "%f", t->value.floatn);
				break;
			}
			case lx::token_type::SIGNED:
			{
				count = khuneo::sformat(out, "%d", t->value.signedn);
				break;
			}
			case lx::token_type::TOKEN:
			{
				count = khuneo::sformat(out, "%c", t->value.token);
				break;
			}
			case lx::token_type::UNSIGNED:
			{
				count = khuneo::sformat(out, "0x%X", t->value.unsignedn);
				break;
			}
			case lx::token_type::STRING:
			case lx::token_type::SYMBOL:
			{
				if (!info)
				{
					khuneo::sformat(out, "%s", "Lexer's run information is required for this token type (string/symbol).");
					return false;
				}

				bool ok = false;
				for (int i = 0; i < t->value.arb.size && i < sz - 1; ++i)
				{
					out[i] = info->start[t->value.arb.rsource + i];
					if (i == t->value.arb.size - 2)
					{
						out[i + 1] = '\0';
						ok = true;
					}
				}
				return ok;
				break;
			}
			default:
				count = khuneo::sformat(out, "%s", "Could not be determined.");
				break;
		}

		return count > 0 && count < sz;
	}

	template <int sz, typename lexer_impl>
	constexpr auto token_sloc_to_string(char (&out)[sz], const khuneo::compiler::lexer::token_node<lexer_impl> * t) -> bool
	{
		char column[16] = { "*" };
		char line[16]   = { "*" };
		if constexpr (requires { t->column; })
		{
			int _count = khuneo::sformat(column, "%d", t->column);
			if (_count <= 0 || _count >= sz)
				return false;
		}

		if constexpr (requires { t->line; })
		{
			int _count = khuneo::sformat(line, "%d", t->line);
			if (_count <= 0 || _count >= sz)
				return false;
		}

		int count = khuneo::sformat(out, "%s, %s", line, column);

		return count > 0 && count < 0;
	}

	#endif
}