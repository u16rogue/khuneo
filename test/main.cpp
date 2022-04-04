#include <khuneo/khuneo.hpp>
#include <cstdio>

auto test_parse_any() -> bool
{
	char cbuff1[] = { "foo is the new bar" };
	char cbuff2[] = { "bar is the new foo" };

	const char * eocb1 = &cbuff1[sizeof(cbuff1)];
	auto pc = khuneo::parser::impl::parse_context<char>(cbuff1, eocb1);

	// Expected: Check -> true, match_length = 3
	{
		auto pr = khuneo::parser::impl::parse_response();
		if (!khuneo::parser::impl::any<"foo">::check(pc, pr) || pr.any.match_length != 3)
			return false;
	}

	// Expected: Check -> false
	{
		auto pr = khuneo::parser::impl::parse_response();
		if (khuneo::parser::impl::any<"bar">::check(pc, pr))
			return false;
	}

	// Expected: Check -> true, match_length = 3
	{
		auto pr = khuneo::parser::impl::parse_response();
		if (!khuneo::parser::impl::any<"foo", "bar">::check(pc, pr) || pr.any.match_length != 3)
			return false;
	}

	// Expected: Check -> true, match_length = 3
	{
		const char * eocb2 = &cbuff2[sizeof(cbuff2)];
		auto pc = khuneo::parser::impl::parse_context<char>(cbuff2, eocb2);
		auto pr = khuneo::parser::impl::parse_response();
		if (!khuneo::parser::impl::any<"foo", "bar">::check(pc, pr) || pr.any.match_length != 3)
			return false;
	}

	return true;
}

#define CREATE_TEST(x) \
	printf("\nRunning test for " #x "..."); \
	if (!x()) \
	{ \
		printf(" Failed!"); \
		return 1; \
	} \
	printf(" Passed!")

auto main() -> int
{
	CREATE_TEST(test_parse_any);

	return 0;
}