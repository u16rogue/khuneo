#pragma once

#include <cstdint>

#include "khuneo_state.hpp"
#include "khuneo_data.hpp"

namespace khuneo::vm
{
	enum class exceptions
	{
		INVALID_OPCODE,
		NO_INTERRUPT_HANDLER,
		INVALID_INTERRUPT_CODE,
	};
}

namespace khuneo::vm::impl
{
	// The current context that the VM uses.
	// Similar to the concept of where each thread in a system has its own state, storage, registers, etc...
	struct context
	{
		khuneo::state * state { nullptr };
		void(*interrupt_handler)(context &);
		void(*exception_handler)(context &, exceptions);

		struct
		{
			khuneo::impl::kh_data_store ip  { 0 }; // instruction pointer
			char interrupt_flag { 0 };

			union
			{
				struct
				{
					khuneo::impl::kh_data_store r0; // General register 0
					khuneo::impl::kh_data_store r1; // General register 1
					khuneo::impl::kh_data_store r2; // General register 2
					khuneo::impl::kh_data_store r3; // General register 3
				};

				khuneo::impl::kh_data_store r[];
			};
		} registers;
	};

	struct register_id
	{
		enum
		{

		};
	};
}