#pragma once

#include <khuneo/string_literals.hpp>
#include <khuneo/compiler/info.hpp>

namespace khuneo::impl::lexer
{
	/*
	* Checks if the current source contains one of
	* the string delimeters. When it successfuly
	* matches one of the delimeters the
	* response value is set to the length of
	* the matched delimeter.
	* 
	* ! Modifies response
	*/
	template <khuneo::string_literal... delims>
	struct streq
	{
		static auto run(impl::info * info) -> bool
		{
			if ((([&]() -> bool
			{
				if (info->check_current_overflow(delims.length - 1))
					return false;

				if (!delims.match(info->state.source))
					return false;

				info->response.value = delims.length;
				return true;

			} ()) || ...)) { return true; }
			
			return false;
		}
	};

	/*
	* Checks if the current state is within
	* the character range of start and end.
	* If the character is in range the response value
	* is set to the index offset of the matched
	* character starting from the start delimeter
	* 
	* ! Modifies response
	*/
	template <char start, char end>
	struct rangechar
	{
		static auto run(impl::info * info) -> bool
		{
			if (info->check_current_overflow(0))
				return false;

			const char c = *info->state.source;
			if (c < start || c > end)
				return false;

			info->response.value = c - start;
			return true;
		}
	};

	/*
	* Moves the current source by an offset, if offset
	* is 0 it instead ues the request's value and
	* then returns true if the source was moved without 
	* overflowing
	* 
	* ! May Use requests
	* ! Modifies current state
	* ! Modifies the text's line and column
	*/
	template <int offset = 0>
	struct forward_source
	{
		static auto run(impl::info * info) -> bool
		{
			int _value = offset != 0 ? offset : info->request.value;

			const char * next = info->state.source + _value;
			if (next - 1 >= info->ctx.end)
				return false;

			while (info->state.source != next)
			{
				switch (*info->state.source)
				{
					case '\t':
						info->state.column += info->ctx.tab_space;
						break;
					case '\n':
						info->state.column = 0;
						++info->state.line;
						break;
					default:
						++info->state.column;
				};

				++info->state.source;
			}

			return true;
		}
	};

	/*
	* Evaluates its expressions if <condition> returns
	* true. If the <condition> is not met kh_if will
	* return true without evaluating
	*/
	template <typename condition, typename... expressions>
	struct kh_if
	{
		static auto run(impl::info * info) -> bool
		{
			return !condition::run(info) || (expressions::run(info) && ...);
		}
	};

	/*
	* Evaluates all expressions if each results
	* to true
	*/
	template <typename... expressions>
	struct kh_and
	{
		static auto run(impl::info * info) -> bool
		{
			return (expressions::run(info) && ...);
		}
	};

	/*
	* Evaluates if any expression results
	* to true
	*/
	template <typename... expressions>
	struct kh_or
	{
		static auto run(impl::info * info) -> bool
		{
			return (expressions::run(info) || ...);
		}
	};

	/*
	* Continously evaluate an expression aslong as
	* condition results to true. This will always
	* return true regardless.
	*/
	template <typename condition, typename expression>
	struct kh_while
	{
		static auto run(impl::info * info) -> bool
		{
			while (condition::run(info))
				expression::run(info);

			return true;
		}
	};

	/*
	* Matches an encapsulated block determined by
	* the <begin> and <end> delimeters. This expression
	* returns true when its able to match both begin and end,
	* and will modify the state's response to a value that
	* represents the length of the entiree encapsulation block
	* starting from the <begin> delimeter up to the <end>
	* 
	* ! Modifies response
	* 
	* ? Maybe this should take expressions instead of a string_literal
	*/
	template <khuneo::string_literal begin, khuneo::string_literal end>
	struct encapsulate
	{
		static auto run(impl::info * info) -> bool
		{
			const char * current = info->state.source;
			if (info->check_current_overflow(begin.length) || !begin.match(current))
				return false;

			++current; // advance the current since we already matched it
			int scope = 0;

			while (!info->check_overflow(current))
			{
				// increment the scope
				if (begin.match(current))
				{
					++scope;
					++current;
					continue;
				}

				if (end.match(current))
				{
					if (scope == 0)
					{
						info->response.value = current - info->state.source + 1;
						return true;
					}

					--scope;
				}

				++current;
			}

			return false;
		}
	};

	/*
	* Inserts a new node and updates the node
	* with information pushed by push_basic_state
	* 
	* ! pops the info stack
	*/
	template <khuneo::string_literal tok>
	struct pop_token_next
	{
		static auto run(impl::info * info) -> bool
		{
			// Find a basic state
			info_stack_entry * stack = info->find_recent(impl::info_stack_type::BASIC_STATE);
			if (!stack)
			{
				info->generate_exception("pop_token_next did not find a BASIC_STATE to use");
				return false;
			}

			ast::node * n = nullptr;

			if (info->state.node->occupied)
			{
				n = info->h_allocate_node();
				if (!n)
				{
					info->generate_exception("Memory allocation failed when trying to insert a token");
					return false;
				}

				info->state.node->link_next(n);

				// update state
				info->state.node = n;
			}
			else
			{
				n = info->state.node;
			}

			// set info
			n->tok_id   = tok.hash;
			n->tok_name = tok.str;
			n->start    = stack->basic_state.start;
			n->end      = info->state.source;
			n->line     = stack->basic_state.line;
			n->column   = stack->basic_state.column;
			n->occupied = true;

			return true;
		}
	};

	/*
	* Pushes the current context to be used
	* by pop_token_next, this is used for referencing
	* like the start and end of the token
	* 
	* ! pushes to the info stack
	*/
	struct push_basic_state
	{
		static auto run(impl::info * info) -> bool
		{
			info->push(info_stack_type::BASIC_STATE);
			return true;
		}
	};

	/*
	* Pushes an exception into the info
	* stack, if an expression fails and
	* an exception is left in the stack
	* the parser will halt and will throw
	* the latest exception pushed
	* 
	* ! pushes to the info stack
	*/
	template <khuneo::string_literal msg>
	struct push_exception
	{
		static auto run(impl::info * info) -> bool
		{
			info->push(info_stack_type::EXCEPTION, (void *)msg.str);
			return true;
		}
	};

	/*
	* Pops the top of an info stack
	* 
	* ! pops the info stack
	*/
	struct pop
	{
		static auto run(impl::info * info) -> bool
		{
			info->pop();
			return true;
		}
	};

	/*
	* Creates a child node on the current
	* node and changes the state's node to
	* the child's group
	*/
	struct start_child
	{
		static auto run(impl::info * info) -> bool
		{
			if (!info->state.node->occupied)
			{
				info->generate_exception("Creating child node on an unoccupied node!");
				return false;
			}

			if (auto * c = info->state.node->child; c)
			{
				info->state.node->link_child(c);
				info->state.node = c;
				return true;
			}

			ast::node * n = info->h_allocate_node();
			if (!n)
			{
				info->generate_exception("Failed to allocate memory when creating a new child node");
				return false;
			}

			info->state.node->link_child(n);
			info->state.node = n;

			return true;
		}
	};

	/*
	* Exits the current child node and reverts
	* back to the child's parent node group
	*/
	struct end_child
	{
		static auto run(impl::info * info) -> bool
		{
			ast::node * n = info->state.node;

			do
			{
				if (n->parent)
				{
					info->state.node = n->parent;
					return true;
				}

				if (!n->prev)
					break;

				n = n->prev;
			} while(n);

			info->generate_exception("end_child is unable to look for a parent node to go back to");
			return false;
		}
	};

	// Helpers 

	template <typename condition>
	using h_gulp_char = kh_while<condition, forward_source<1>>;

	template <typename condition>
	using h_gulp = kh_while<condition, forward_source<>>;

	using h_spacingchars = streq<" ", "\n", "\t", "\r">;

	using h_match_AZaz_$   = kh_or< rangechar<'A', 'Z'>, rangechar<'a', 'z'>, streq<"_", "$"> >;
	using h_match_AZaz_$09 = kh_or< h_match_AZaz_$,      rangechar<'0', '9'> >;

	using h_gulp_whitespace = h_gulp_char< h_spacingchars >;

	using symbol = kh_and
	<
		push_exception<"Expected a symbol/identifier">,
		h_match_AZaz_$,
		push_basic_state,
		forward_source<1>,
		h_gulp_char<h_match_AZaz_$09>,
		pop_token_next<"SYMBOL">,
		pop
	>;
}

namespace khuneo::lexer
{
}