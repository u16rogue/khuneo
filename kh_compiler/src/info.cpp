#include <khuneo/compiler/info.hpp>

auto khuneo::impl::parser::info::check_overflow(const char * c) -> bool
{
	return c >= ctx.end;
}

auto khuneo::impl::parser::info::check_current_overflow(const int & diff) -> bool
{
	return check_overflow(state.source + diff);
}

auto khuneo::impl::parser::info::generate_exception(const char * message) -> void
{
	stack_push(info_stack_type::EXCEPTION, (void *)message);
}

auto khuneo::impl::parser::info::h_allocate_node() -> ast::node *
{
	ast::node * n = reinterpret_cast<ast::node *>(ctx.kh_alloc(sizeof(ast::node)));
	if (n)
	{
		*n = {};
		n->occupied = false;
	}

	return n;
}

auto khuneo::impl::parser::info::stack_top() -> info_stack_entry &
{
	return stack[stack_counter];
}

auto khuneo::impl::parser::info::stack_push(info_stack_type type, void * extra_data) -> bool
{
	if (type == info_stack_type::UNDEFINED || stack_counter + 1 == sizeof(stack) / sizeof(stack[0]))
		return false;

	++stack_counter;
	info_stack_entry & entry = stack[stack_counter];
	entry.type = type;

	switch (type)
	{
		case info_stack_type::BASIC_STATE:
		{
			entry.basic_state.start  = state.source;
			entry.basic_state.line   = state.line;
			entry.basic_state.column = state.column;
			break;
		}
		case info_stack_type::NUMBER:
		{
			entry.number = *reinterpret_cast<int*>(&extra_data);
			break;
		}
		case info_stack_type::EXCEPTION:
		{
			entry.exception.info_instance = this;
			entry.exception.message = reinterpret_cast<const char *>(extra_data);
			entry.exception.line    = state.line;
			entry.exception.column  = state.column;
			break;
		}
		default:
		{
			stack_pop();
			return false;
			break;
		}
	}

	return true;
}

auto khuneo::impl::parser::info::stack_pop() -> bool
{
	stack[stack_counter] = {};

	if (stack_counter == 0)
		return false;

	--stack_counter;
	return true;
}

auto khuneo::impl::parser::info::stack_count() -> int
{
	return stack_counter;
}

auto khuneo::impl::parser::info::stack_indexed(int i) -> info_stack_entry &
{
	return stack[i];
}

auto khuneo::impl::parser::info::stack_find_recent(info_stack_type type) -> info_stack_entry *
{
	int s = stack_counter;
	while (s)
	{
		if (stack[s].type == type)
			return &stack[s];
		--s;
	}

	return nullptr;
}
