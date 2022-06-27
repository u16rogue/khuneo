#pragma once

namespace khuneo
{
	// Warning: NOT NULL TERMINATED!
	template <int sz>
	struct string_literal
	{
		consteval string_literal(const char (&s)[sz])
		{
			for (int i = 0; i < sz - 1; ++i)
				data[i] = s[i];
		}

		constexpr auto length() const -> int { return sz - 1; }
		auto get_data() const -> const char * { return data; }

		char data[sz - 1] { 0 };
	};
}