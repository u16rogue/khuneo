﻿#pragma once

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
			int _value = [&]() -> int {
				if constexpr (offset != 0)
					return offset;
				else
					return info->request.value;
			}();

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

	template <khuneo::string_literal begin, khuneo::string_literal end>
	struct encapsulate
	{
		static auto run(impl::info * info) -> bool
		{
			return true;
		}
	};

	/*
	* Inserts a new node and updates the node
	* with information pushed by begin_token
	* 
	* ! pops the info stack
	*/
	template <khuneo::string_literal tok>
	struct insert_token
	{
		static auto run(impl::info * info) -> bool
		{
			ast::node * n = (ast::node *)info->ctx.allocator(sizeof(ast::node));
			if (!n)
				return false;
			
			// set info
			const auto & stack = info->top();
			n->tok_id   = tok.hash;
			n->tok_name = tok.str;
			n->start    = stack.basic_state.start;
			n->end      = info->state.source;
			n->line     = stack.basic_state.line;
			n->column   = stack.basic_state.column;
			info->pop();

			info->state.node->link_forward(n);

			// update state
			info->state.node = n;

			return true;
		}
	};

	/*
	* Pushes the current context to be used
	* by insert_token, this is used for referencing
	* like the start and end of the token
	* 
	* ! pushes to the info stack
	*/
	struct begin_token
	{
		static auto run(impl::info * info) -> bool
		{
			info->push(info_stack_type::BASIC_STATE);
			return true;
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

	using identifier = kh_and
	<
		begin_token,
		h_match_AZaz_$,
		forward_source<1>,
		h_gulp_char<h_match_AZaz_$09>,
		insert_token<"IDENTIFIER">
	>;
}

namespace khuneo::lexer
{
	

	// --------------------------------------------------------------------------------------------------------------------------------------------------------

	using expr_moduleexport = impl::lexer::kh_and
	<
		impl::lexer::streq<"export as ">,
		impl::lexer::begin_token,
		impl::lexer::insert_token<"EXPORT_MODULE">,
		impl::lexer::forward_source<>,
		impl::lexer::h_gulp_whitespace,
		impl::lexer::identifier,
		impl::lexer::h_gulp_whitespace,
		impl::lexer::streq<";">,
		impl::lexer::forward_source<1>
	>;

	using importexpr = impl::lexer::kh_and
	<
		impl::lexer::streq<"import ">,
		impl::lexer::forward_source<>,
		impl::lexer::h_gulp_whitespace
	>;
}