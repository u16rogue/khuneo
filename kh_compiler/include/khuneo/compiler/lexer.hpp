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
		static auto parse(impl::info * info) -> bool
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
		static auto parse(impl::info * info) -> bool
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
	template <int offset = 0, int additive = 0>
	struct forward_source
	{
		static auto parse(impl::info * info) -> bool
		{
			int _value = offset != 0 ? offset : info->request.value;

			const char * next = info->state.source + (_value + additive);
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
	* Checks the current source
	* if it goes over the end
	*/
	template <int offset>
	struct check_end
	{
		static auto parse(impl::info * info) -> bool
		{
			return info->state.source + offset >= info->ctx.end;
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
		static auto parse(impl::info * info) -> bool
		{
			return !condition::parse(info) || (expressions::parse(info) && ...);
		}
	};

	/*
	* Evaluates all expressions if each results
	* to true
	*/
	template <typename... expressions>
	struct kh_and
	{
		static auto parse(impl::info * info) -> bool
		{
			return (expressions::parse(info) && ...);
		}
	};

	/*
	* Evaluates if any expression results
	* to true
	*/
	template <typename... expressions>
	struct kh_or
	{
		static auto parse(impl::info * info) -> bool
		{
			return (expressions::parse(info) || ...);
		}
	};

	/*
	* Continously evaluate an expression aslong as
	* condition results to true. This will always
	* return true regardless.
	*/
	template <typename condition, typename expression = void>
	struct kh_while
	{
		static auto parse(impl::info * info) -> bool
		{
			while (condition::parse(info))
				if constexpr (requires { expression::parse(nullptr); })
					expression::parse(info);

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
		static auto parse(impl::info * info) -> bool
		{
			const char * current = info->state.source;
			if (info->check_current_overflow(begin.length) || !begin.match(current))
				return false;

			++current; // advance the current since we already matched it
			int scope = 0;

			while (!info->check_overflow(current))
			{
				if (end.match(current))
				{
					if (scope == 0)
					{
						info->response.value = current - info->state.source + 1;
						return true;
					}

					--scope;
				}

				// increment the scope
				if (begin.match(current))
				{
					++scope;
					++current;
					continue;
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
		static auto parse(impl::info * info) -> bool
		{
			// Find a basic state
			info_stack_entry * stack = info->stack_find_recent(impl::info_stack_type::BASIC_STATE);
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
		static auto parse(impl::info * info) -> bool
		{
			info->stack_push(info_stack_type::BASIC_STATE);
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
		static auto parse(impl::info * info) -> bool
		{
			info->stack_push(info_stack_type::EXCEPTION, (void *)msg.str);
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
		static auto parse(impl::info * info) -> bool
		{
			info->stack_pop();
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
		static auto parse(impl::info * info) -> bool
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
		static auto parse(impl::info * info) -> bool
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

	/*
	* Negates an expression
	*/
	template <typename expr>
	struct negate
	{
		static auto parse(impl::info * info) -> bool
		{
			return !expr::parse(info);
		}
	};

	/*
	* Runs a parser on the current node
	* The offset value is used to skip the number of
	* characters when applying the <start> and <end>
	* value of the node that's being parsed. The expressions
	* variadic parameter can be given a set of expressions to
	* be used for parsing the child node, if none is provided
	* the current parser in the context is used.
	* 
	* ! default offset value is 1.
	*	-> in cases of "(foo)" the default value will treat it
	*      as "foo" skipping over the opening and closing
	*      parenthesis.
	*/
	template <int offset = 1, typename... expressions> 
	struct parse_child
	{
		static auto parse(impl::info * info) -> bool
		{
			impl::info i {};
			i.ctx.kh_alloc = info->ctx.kh_alloc;
			i.ctx.start     = info->state.node->start + offset;
			i.ctx.end       = info->state.node->end - offset;
			i.ctx.parser_exception = info->ctx.parser_exception;
			i.ctx.tab_space = info->ctx.tab_space;
			i.state.source  = i.ctx.start;
			i.state.line    = info->state.node->line;
			i.state.column  = info->state.node->column + offset;

			// create a new child node
			auto * pnode = info->h_allocate_node();
			if (!pnode)
			{
				info->generate_exception("Failed to allocate node for parsing a child node");
				return false;
			}
			
			// set the child's root node as the newly created one
			i.ctx.root_node = pnode;
			i.state.node    = i.ctx.root_node;

			// link the new node as a child to our main current node
			info->state.node->link_child(pnode);
			
			if constexpr (sizeof...(expressions) == 0)
				return info->ctx.parser(&i);
			else
				return (expressions::parse(&i) && ...);
		}	
	};

	/*
	* Executes a lambda procedure
	*/
	template <auto lambda>
	struct proc
	{
		static auto parse(impl::info * info) -> bool
		{
			return lambda(info);
		}
	};

	// Helpers 

	template <typename condition>
	using h_gulp_char = kh_while<kh_and<condition, forward_source<1>>>;

	template <typename condition>
	using h_gulp = kh_while<condition, forward_source<>>;

	using h_spacingchars = streq<" ", "\n", "\t", "\r">;

	using h_match_AZaz_$   = kh_or< rangechar<'A', 'Z'>, rangechar<'a', 'z'>, streq<"_", "$"> >;
	using h_match_AZaz_$09 = kh_or< h_match_AZaz_$,      rangechar<'0', '9'> >;

	using h_gulp_whitespace = h_gulp_char< h_spacingchars >;
}

namespace khuneo::lexer
{
}