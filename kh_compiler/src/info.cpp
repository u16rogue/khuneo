#include <khuneo/compiler/info.hpp>

auto khuneo::impl::info::check_overflow(const char * c) -> bool
{
	return c >= ctx.end;
}

auto khuneo::impl::info::check_current_overflow(const int & diff) -> bool
{
	return check_overflow(state.source + diff);
}

auto khuneo::impl::info::push_tokend() -> void
{
	token_data.start  = state.source;
	token_data.line   = state.line;
	token_data.column = state.column;
}

auto khuneo::impl::info::pop_tokend(ast::node * n) -> void
{
	n->line   = token_data.line;
	n->column = token_data.column;
	n->start  = token_data.start;
	n->end    = state.source;
}
