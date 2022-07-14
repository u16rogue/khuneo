#pragma once

#include <cstdio>

#ifdef KH_EXTRA_COMPILER_AVAILABLE
	#include <khuneo/compiler/lexer.hpp>
#endif

// TODO: Make our own snprintf

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
	constexpr auto msg_format(char (&out)[sz], khuneo::compiler::lexer::msg_callback_info<lexer_impl> * m) -> bool
	{
		bool is_fatal = khuneo::compiler::lexer::is_msg_fatal(m->message);
		const char * m_type = is_fatal ? "FATAL" : "WARNING";

		if constexpr (lexer_impl::enable_sloc_track)
		{
			snprintf(out, sz, "[%s | %d,%d] %s",
					 m_type,
					 m->info->line,
					 m->info->column,
					 msg_string(m->message) 
			);
		}
		else
		{
			snprintf(out, sz, "[%s] %s",
					 m_type,
					 msg_string(m->message) 
			);
		}

		return true;
	}

	#endif
}