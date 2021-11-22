#pragma once

namespace khuneo
{
	#define KHUNEO_HAS_IMPL_PARAMETERS template <class... impl_params>
	#define KHUNEO_IMPL_PARAMETERS imple_params...

	struct set_parameter
	{

	};

	KHUNEO_HAS_IMPL_PARAMETERS
	struct parameter
	{
	private:

	public:
		template <typename T>
		consteval auto get() -> T
		{

		}
	};
}