#include <khuneo/khuneo.hpp>
#include <cstdio>

using namespace khuneo::parser::impl;

// TODO: cleanup test

class print_test
{
public:
	print_test(const char * msg)
		: msg(msg)
	{
		printf("\n\t> %s... ", msg);
	}

	~print_test()
	{
		printf(pass ? "Passed!" : "Failed!");
	}

	auto passed() noexcept -> print_test &
	{
		pass = true;
		return *this;
	}

	operator bool() const noexcept
	{
		return pass;
	}

private:
	const char * const msg;
	bool pass { false };
};

#define TEST(x) auto _test = print_test(x) 

#define TEST_EXPECT_OK(exp) \
	if ((exp) == false) \
		return false; \
	_test.passed()

#define TEST_EXPECT_FAIL(exp) \
	if ((exp) == true) \
		return false; \
	_test.passed()

auto test_parse_ternary() -> bool
{
	char buff[] = { "FOOBAR" };
	auto pc = parse_context<char>(buff, &buff[sizeof(buff)]);
	auto pr = parse_response();

	TEST("Test ternary exact match");
	TEST_EXPECT_OK((ternary< any<"BAR">, exact<any<"BARFOO">>, exact<any<"FOOBAR">> >::parse(&pc, &pr)));

	return true;
}

auto test_parse_conditional() -> bool
{
	char buff[] = { "FOOBAR" };
	auto pc = parse_context<char>(buff, &buff[sizeof(buff)]);
	auto pr = parse_response();

	TEST("Test conditional exact match");
	TEST_EXPECT_OK((conditional< any<"FOO">, exact<any<"FOOBAR">> >::parse(&pc, &pr)));

	return true;
}

auto test_parse_exact() -> bool
{
	char buff[] = { "FOOBAR" };
	auto pc = parse_context<char>(buff, &buff[sizeof(buff)]);
	auto pr = parse_response();

	TEST("Test exact match");
	TEST_EXPECT_OK((exact< any<"FOOBAR"> >::parse(&pc, &pr)));

	return true;
}

auto test_parse_gulp() -> bool
{
	char buff[] = { "FOObar    {}" };
	auto pc = parse_context<char>(buff, &buff[sizeof(buff)]);
	auto pr = parse_response();

	TEST("Gulping test expression with any, range");

	TEST_EXPECT_OK((gulp<
						any<"FOO", " ", "{", "}">,
						range<'a', 'z'>,
						any<" ">
					>::parse(&pc, &pr)
					&& pr.gulp.consumed_count == sizeof(buff) - 1));

	
	return true;
}

auto test_parse_kh_or() -> bool
{
	char buff[8] = { "FOOBAR" };
	auto pc = parse_context<char>(buff, &buff[sizeof(buff)]);
	auto pr = parse_response();

	{
		TEST("Test parser OR uppercase letter range expression on uppercase and lowercase");
		TEST_EXPECT_OK((kh_or< range<'A', 'Z'>, range<'a', 'z'> >::parse(&pc, &pr)));
	}

	{
		TEST("Test parser OR uppercase letter range expression on numerics and lowercase");
		TEST_EXPECT_FAIL((kh_or< range<'0', '9'>, range<'a', 'z'> >::parse(&pc, &pr)));
	}

	return true;
}

auto test_parse_kh_and() -> bool
{
	char buff[8] = { "FOOBAR" };
	auto pc = parse_context<char>(buff, &buff[sizeof(buff)]);
	auto pr = parse_response();

	{
		TEST("Test parser AND capital range and phrase match");
		TEST_EXPECT_OK((kh_and< range<'A', 'Z'>, any<"FOO"> >::parse(&pc, &pr)));
	}

	{
		TEST("Test parser AND capital range and phrase mismatch");
		TEST_EXPECT_FAIL((kh_and< range<'A', 'Z'>, any<"BAR"> >::parse(&pc, &pr)));
	}

	return true;
}

auto test_parse_range() -> bool
{
	
	char buff[8] = { "CAR" };
	auto pc = parse_context<char>(buff, &buff[sizeof(buff)]);
	auto pr = parse_response();

	{
		TEST("Test range match A to Z with match to C");
		TEST_EXPECT_OK((range<'A', 'Z'>::parse(&pc, &pr) && pr.range.index == 'C' - 'A'));
	}

	{
		TEST("Test range mismatch a to z with match to C");
		TEST_EXPECT_FAIL((range<'a', 'z'>::parse(&pc, &pr)));
	}
	

	return true;
}

auto test_parse_skip() -> bool
{
	TEST("Skipping multiple delimeters");
	char buff[50] = { "  ... FOO BARZ" };
	auto pc = parse_context<char>(buff, &buff[sizeof(buff)]);
	auto pr = parse_response();

	skip< any<" ", ".", "FOO"> >::parse(&pc, &pr);

	if (*(int *)pc.current != *(int*)"BARZ")
		return false;

	_test.passed();
	return true;
}

auto test_parse_negate() -> bool
{
	{
		TEST("Test negate parse result true -> false");
		char buff[50] = { "foo" };
		auto pc = parse_context<char>(buff, &buff[50]);
		auto pr = parse_response();

		if (negate< any<"foo"> >::parse(&pc, &pr) != false)
			return false;

		_test.passed();
	}

	{
		TEST("Test negate parse result false -> true");
		char buff[50] = { "bar" };
		auto pc = parse_context<char>(buff, &buff[50]);
		auto pr = parse_response();

		if (negate< any<"foo"> >::parse(&pc, &pr) != true)
			return false;

		_test.passed();
	}


	return true;
}

auto test_parse_encapsulated() -> bool
{
	struct entry_check
	{
		const char * prompt;
		int expected_block_size;
		bool expected_result;
		char * buffer;
	};

	constexpr int buffer_size = 50;

	char buff_valid[buffer_size]       = { "{ foo }" };
	char buff_nestedvalid[buffer_size] = { "{ {foo} }" };
	char buff_validextra[buffer_size]  = { "{ foo } }" };
	char buff_invalidnest[buffer_size] = { "{ { foo } " };
	char buff_validmultidelim[buffer_size] = { "< { foo > > } " };

	entry_check checks[] =
	{
		{ "Test valid scope",                                      7,  true,  buff_valid           },
		{ "Test valid nested scope",                               9,  true,  buff_nestedvalid     },
		{ "Test valid scope with extra invalid end scope",         7,  true,  buff_validextra      },
		{ "Test invalid nested buffer (missing parent scope end)", 0,  false, buff_invalidnest     },
		{ "Test valid scope with different delimeter",             11, true,  buff_validmultidelim },
	};

	for (const auto check : checks)
	{
		TEST(check.prompt);
		auto pc = khuneo::parser::impl::parse_context<char>(check.buffer, &check.buffer[buffer_size]);
		auto pr = khuneo::parser::impl::parse_response();

		if (encapsulated< any<"{", "<">, any<"}", ">"> >::parse(&pc, &pr) == check.expected_result && (!check.expected_result || check.expected_result && pr.encapsulated.block_size == check.expected_block_size))
		{
			_test.passed();
			continue;
		}

		return false;
	}

	return true;
}

auto test_parse_any() -> bool
{
	char cbuff1[] = { "foo is the new bar" };
	char cbuff2[] = { "bar is the new foo" };

	const char * eocb1 = &cbuff1[sizeof(cbuff1)];
	auto pc = khuneo::parser::impl::parse_context<char>(cbuff1, eocb1);

	// Expected: Check -> true, match_length = 3
	{
		TEST("Test match 'foo'");
		auto pr = khuneo::parser::impl::parse_response();
		if (!khuneo::parser::impl::any<"foo">::parse(&pc, &pr) || pr.any.match_length != 3)
			return false;

		_test.passed();
	}

	// Expected: Check -> false
	{
		TEST("Test mismatch 'bar'");
		auto pr = khuneo::parser::impl::parse_response();
		if (khuneo::parser::impl::any<"bar">::parse(&pc, &pr))
			return false;

		_test.passed();
	}

	// Expected: Check -> true, match_length = 3
	{
		TEST("Test match multiple as 'foo'");
		auto pr = khuneo::parser::impl::parse_response();
		if (!khuneo::parser::impl::any<"foo", "bar">::parse(&pc, &pr) || pr.any.match_length != 3)
			return false;

		_test.passed();
	}

	// Expected: Check -> true, match_length = 3
	{
		TEST("Test match multiple as 'bar'");
		const char * eocb2 = &cbuff2[sizeof(cbuff2)];
		auto pc = khuneo::parser::impl::parse_context<char>(cbuff2, eocb2);
		auto pr = khuneo::parser::impl::parse_response();
		if (!khuneo::parser::impl::any<"foo", "bar">::parse(&pc, &pr) || pr.any.match_length != 3)
			return false;

		_test.passed();
	}

	return true;
}

#define RUN_TEST(x) \
	printf("\n[#] Running test for: " #x); \
	if (!x()) \
	{ \
		return 1; \
	}

auto main() -> int
{
	RUN_TEST(test_parse_any);
	RUN_TEST(test_parse_encapsulated);
	RUN_TEST(test_parse_negate);
	RUN_TEST(test_parse_skip);
	RUN_TEST(test_parse_range);
	RUN_TEST(test_parse_kh_or);
	RUN_TEST(test_parse_kh_and);
	RUN_TEST(test_parse_gulp);
	RUN_TEST(test_parse_exact);
	RUN_TEST(test_parse_conditional);
	RUN_TEST(test_parse_ternary);

	return 0;
}