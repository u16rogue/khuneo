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
					khuneo::impl::kh_data_store r0;
					khuneo::impl::kh_data_store r1;
					khuneo::impl::kh_data_store r2;
					khuneo::impl::kh_data_store r3;
					khuneo::impl::kh_data_store r4;
					khuneo::impl::kh_data_store r5;
					khuneo::impl::kh_data_store r6;
					khuneo::impl::kh_data_store r7;
					khuneo::impl::kh_data_store r8;
					khuneo::impl::kh_data_store r9;
					khuneo::impl::kh_data_store r10;
					khuneo::impl::kh_data_store r11;
					khuneo::impl::kh_data_store r12;
					khuneo::impl::kh_data_store r13;
					khuneo::impl::kh_data_store r14;
					khuneo::impl::kh_data_store r15;
				};

				khuneo::impl::kh_data_store r[16];
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