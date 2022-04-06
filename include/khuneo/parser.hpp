#pragma once

// #include <type_traits> // TODO: check if this library can compile everywhere, if not implement our own SFINAE
#include "string_literals.hpp"

namespace khuneo::parser::impl
{
	/*
	* It is a general standard that parser implementation should implement
	* a static function named "parse" that returns a boolean value and takes
	* two parameter, a parse_context that is used to track the movement of
	* the parser and a parse_response parameter to return the generated result
	* of the parser implementation.
	* 
	* <implementation>::parse(<parse_context>, <parse_response>);
	* 
	* both parameters should be a reference as such it should have the ampersand
	* operator on its declaration. the parse_context should be marked const as
	* much as possible if the parser implementation does not cause any side effect
	* 
	* If a parser implementation needs to pass information its data should be
	* implemented and passed through the parse_response union by implementing its
	* own anonymous struct. ~~If a third party wishes to expand the parse_response
	* struct to be able to add their own parser implementation it can be provided through
	* the parse_response's extension typename~~ (the ~~ are supposed to be strike through,
	* but it seems to not render properly)
	* 
	* TODO: document <implementation>::longest()
	*/

	template <typename T_sourcebuffer = char>
	struct parse_context
	{
		parse_context(const T_sourcebuffer * _current, const T_sourcebuffer * _end)
			: current(_current), end(_end)
		{}

		const T_sourcebuffer * current;
		const T_sourcebuffer * const end;
	};

	/*
	* Used by parser implementations to pass information back to its caller,
	* A parser implementation should have its own anonymous struct inside
	* the union and should limit itself to only modifying the struct for itself
	*/
	union parse_response
	{
		struct
		{
			int match_length; // Length of the matched delimeter
		} any;

		struct
		{
			void * start;        // Pointer to the start of the start identifier match
			int    start_length; // Length of the start matched delimeter

			void * end;          // Pointer to the end of the end identifier match
			int    end_length;   // Length of the end matched delimeter

			int    block_size;   // The size of the entire encapsulation eg. "{}" = 2, "{ }" = 3, "{foo}" = 5
		} encapsulated;
	};

	/*
	* Accepts multiple strings as a variadic template argument
	* which provides context to its parent consumer.
	* 
	* This does simply run a check, no side effects are made
	* by this.
	* 
	* ex. any<"var", "fn">::parse(...);
	*        ┌── Fails
	*        ▼
	*        export as name;
	*   ┌───►fn do() {}
	*   │    var x = 1;
	*   │    ▲
	*   │    └─ Matches and parse_respone::any.match_length = 3
	*   │
	*   └─ Matches and parse_response::any.match_length = 2	 
	*/
	template <khuneo::string_literal... delims>
	struct any
	{
		template <typename T_wc>
		static auto parse(const parse_context<T_wc> & pc, parse_response & resp) -> bool
		{
			if (([&]() -> bool
			{
				// check if there's enough space in the buffer for the match to even take place (prevent matching beyond the buffer)
				if (pc.current + delims.length >= pc.end)
					return false;

				if (delims == pc.current)
				{
					resp.any.match_length = delims.length;
					return true;
				}

				return false;
			} () || ...)) { return true; }

			return false;
		}

		// Returns the longest length
		static consteval auto length() -> int
		{
			int highest { 0 };

			([&]()
			{
				if (delims.length > highest)
					highest = delims.length;
			} (), ...);

			return highest;
		}

		// Returns the lowest length
		static consteval auto minlength() -> int
		{
			int lowest { -1 };

			([&]()
			{
				if (lowest == -1)
					lowest = delims.length;
				else if (delims.length < lowest)
					lowest = delims.length;
			} (), ...);

			return lowest;
		}
	};
	
	struct range
	{

	};

	/*
	*  Negates the result of expression::parse
	*/
	template <typename expression>
	struct negate
	{
		static auto parse(auto & ... args) -> bool
		{
			return !expression::parse(args...);
		}

		// TODO: implement the other static methods
	};

	/*
	* 
	*/
	template <typename... delims>
	struct skip
	{

	};

	struct kh_or
	{
	};

	struct kh_and
	{
	};

	/*
	* Parses the source buffer starting from the supplied start identifier until an
	* end identifier is met. Both start and end typename should be a parsable type
	* where a start::parse and end::parse is a valid expression. This function will
	* skip any other string that doesn't match the end identifier after matching a start
	* identifier. The parse of this will produce side effects to the parse context where
	* it would modify the current to the resulting end match + 1.
	*
    * when parsed with encapsulate<any<"\"">, any<"\"">>::parse(...)
    * 
    *   ┌── parse_response::encapsulated.start ~ start_length = 1
    *   ▼
    * " { }       "
    *      ▲
    *      └── parse_response::encapsulated.end and parse_context::current  ~ end_length = 1
    * 
    * this can also be matched with multiple any<..., ...> for both
    * start and end as such a valid encapsulation expression can be
	* 
    * using foo_or_bar = any<"foo", "bar">;
    * encapsulate<foo_or_bar, any<"foo", "barz">>::parse(...)
    * 
    *   ┌──── parse_response::encapsulated.start ~ start_length = 3
    *   ▼
    * " foo barz  "
    *           ▲
    *           └── parse_response::encapsulated.end & parse_context::current ~ end_length = 4
	* 
	* Side effects are made to the parse_context as encapsulated<..., ...> runs its parser
	* regardless of success or failure, this is designed like this incase of a syntax error
	* we can continue parsing the source buffer to catch other errors
	*/
	template <typename start, typename end>
	struct encapsulated
	{
		template <typename T_wc>
		static auto parse(parse_context<T_wc> & pc, parse_response & resp) -> bool
		{
			if (pc.current + start::minlength() + end::minlength() >= pc.end)
				return false;

			const auto * const o_current = pc.current;

			// Parse the starting point of the encapsulation
			parse_response _resp {};
			if (!start::parse(pc, _resp))
				return false;

			// Update our response for the start
			resp.encapsulated.start        = (void *)pc.current;
			resp.encapsulated.start_length = _resp.any.match_length;

			// Move the context's current past the start delimeter
			pc.current = pc.current + _resp.any.match_length;
			if (pc.current >= pc.end)
				return false;

			_resp = {};
			bool valid = false;
			int nest_scope = 0;

			do
			{
				// Check if entering a new scope
				if (start::parse(pc, _resp))
				{
					++nest_scope;
					pc.current += _resp.any.match_length;
					continue;
				}

				if (end::parse(pc, _resp))
				{
					if (nest_scope == 0)
					{
						valid = true;
						break;
					}
					else
					{
						--nest_scope;
						pc.current += _resp.any.match_length;
						continue;
					}
				}

				++pc.current;
			} while (pc.current < pc.end && *pc.current);

			if (!valid)
				return false;

			pc.current += _resp.any.match_length;
			resp.encapsulated.end_length = _resp.any.match_length;
			resp.encapsulated.end = (void *)pc.current;
			resp.encapsulated.block_size = (int)((unsigned long long)resp.encapsulated.end - (unsigned long long)resp.encapsulated.start); // TODO: clean this up

			return true;
		}
	};

	struct parse_forward
	{

	};

	template <typename... vargs_t>
	struct basic_parser
	{

	};
}