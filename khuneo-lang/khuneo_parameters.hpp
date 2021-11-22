#pragma once

#include <type_traits>
#include <cstdint>
#include "khuneo_hash.hpp"

namespace khuneo::impl
{
	template <std::size_t len>
	struct parameter_string_literal
	{
		consteval parameter_string_literal(const char (&str)[len])
			: value(khuneo::hash<std::uint64_t>(str))
		{
		}

		std::uint64_t value { 0 };
	};

	template <parameter_string_literal name, typename T, T value>
	struct set_parameter
	{
		template <typename to_type>
		consteval static auto compare(std::uint64_t to) -> bool
		{
			return name.value == to && std::is_same_v<T, to_type>;
		}

		consteval static auto get() -> T
		{
			return value;
		}
	};

	template <class... parameter_set>
	struct parameter
	{
	private:
		template <parameter_string_literal name, typename T, class param>
		consteval static auto consume_v() -> T
		{
			if constexpr (param::compare<T>(name.value))
				return param::get();
			else
				return 0; // This should never be reached.
		}

		template <parameter_string_literal name, typename T, class param, class... other_params> requires (sizeof...(other_params) != 0)
			consteval static auto consume_v() -> T
		{
			if constexpr (param::compare<T>(name.value))
				return param::get();
			else
				return consume_v<name, T, other_params...>();
		}

	public:
		template <parameter_string_literal name, typename T>
		consteval static auto has() -> T
		{
			return (parameter_set::compare<T>(name.value) || ...);
		}

		template <parameter_string_literal name, typename T, T default_value = 0>
		consteval static auto get() -> T
		{
			if constexpr (!has<name, T>() || sizeof...(parameter_set) == 0)
				return default_value;
			else
				return consume_v<name, T, parameter_set...>();
		}
	};
}

#define KHUNEO_PARAMETER_TEMPLATE class... __impl_params
#define KHUNEO_PARAMETER __impl_params...

#define KHUNEO_PARAMETER_HAS(name, type) khuneo::impl::parameter<KHUNEO_PARAMETER>::template has<name, type>()
#define KHUNEO_PARAMETER_GET(name, type, default_value) khuneo::impl::parameter<KHUNEO_PARAMETER>::template get<name, type, default_value>();
#define KHUNEO_PARAMETER_SET(name, type, value) khuneo::impl::set_parameter<name, type, value>