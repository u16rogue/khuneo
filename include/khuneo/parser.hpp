#pragma once

#include <type_traits> // TODO: check if this library can compile everywhere, if not implement our own SFINAE
#include "string_literals.hpp"

namespace khuneo::parser::impl
{
	template <typename T_sourcebuffer = char>
	struct parse_context
	{
		parse_context(const T_sourcebuffer * _current, const T_sourcebuffer * _end)
			: current(_current), end(_end)
		{}

		const T_sourcebuffer * current;
		const T_sourcebuffer * const end;
	};

	union parse_response
	{
		struct
		{
			int match_length;
		} any;
	};

	/*
	* Accepts multiple strings as a variadic template argument
	* which provides context to its parent consumer.
	* 
	* This does simply run a check, no side effects are made
	* by this.
	* 
	* ex. any<"var", "fn">::check(...);
	*     ┌── Fails
	*     ▼
	*     export as name;
	*   ┌►fn do() {}
	* ┌─┘►var x = 1;
	* │
	* └─── Matches
	*/
	template <khuneo::string_literal... delims>
	struct any
	{
		template <typename T_wc>
		static auto check(const parse_context<T_wc> & wc, parse_response & resp) -> bool
		{
			if (([&]() -> bool
			{
				// check if there's enough space in the buffer for the match to even take place (prevent matching beyond the buffer)
				if (wc.current + delims.length >= wc.end)
					return false;

				if (delims == wc.current)
				{
					resp.any.match_length = delims.length;
					return true;
				}

				return false;
			} () || ...)) { return true; }

			return false;
		}
	};
	
	struct range
	{

	};

	struct skip
	{

	};

	struct kh_or
	{
	};

	struct kh_and
	{
	};

	template <auto start, auto end>
	struct encapsulated
	{

	};

	struct parse_forward
	{

	};

	template <typename... vargs_t>
	struct basic_parser
	{

	};
}