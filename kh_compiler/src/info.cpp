#include <khuneo/compiler/info.hpp>

auto khuneo::impl::info::check_overflow(const char * c) -> bool
{
	return c >= ctx.end;
}

auto khuneo::impl::info::check_current_overflow(const int & diff) -> bool
{
	return check_overflow(state.source + diff);
}

auto khuneo::impl::info::top() -> info_stack_entry &
{
	return stack[stack_counter];
}

auto khuneo::impl::info::push(info_stack_type type, void * extra_data) -> bool
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
			entry.exception.message = reinterpret_cast<const char *>(extra_data);
			entry.exception.line    = state.line;
			entry.exception.column  = state.column;
		}
		default:
		{
			pop();
			return false;
			break;
		}
	}

	return true;
}

auto khuneo::impl::info::pop() -> bool
{
	stack[stack_counter] = {};

	if (stack_counter == 0)
		return false;

	--stack_counter;
	return true;
}
