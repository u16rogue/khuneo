#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>

#include "khuneo_vm_context.hpp"

namespace khuneo::vm::impl
{
	template <std::size_t len>
	struct opcode_mnenomic
	{
		consteval opcode_mnenomic(const char (&name)[len])
		{
			for (int i = 0; i < len; i++)
			{
				mnenomic[i] = name[i];

				if (i != len - 1)
					code = (code * 0x01000193) ^ name[i]; // FNV-1 hash
			}
		}

		char         mnenomic[len] {  0  };
		unsigned int code          { 0x811c9dc5 };
		std::size_t  length        { len };

		static_assert(sizeof(code) == 4, "khuneo::vm::impl::opcode_mnenomic::code requires to be 4 bytes.");
	};

	#define KHUNEO_CTX ctx                                          // Defines the current context. ONLY USE WHEN KHUNEO_CTX_PARAM is in scope!
	#define	KHUNEO_CTX_PARAM khuneo::vm::impl::context & KHUNEO_CTX // Declares the current context

	template <opcode_mnenomic op, std::size_t operand_size, void(*op_exec)(KHUNEO_CTX_PARAM)>
	struct define_opcode
	{
		enum
		{
			OPERAND_SIZE = operand_size,
			SIZE         = sizeof(decltype(op.code)) + operand_size,
			CODE         = op.code,
		};
		
		static auto check_and_exec(KHUNEO_CTX_PARAM) -> bool
		{
			if (*reinterpret_cast<decltype(op.code)*>(KHUNEO_CTX.registers.ip.ptr) != op.code)
				return false;

			constexpr auto _op_exec = op_exec; // for debugging.
			_op_exec(KHUNEO_CTX);
			return true;
		}

		// Assembles the current opcode to a buffer pointed by place_at
		template <typename ret_t = std::uint8_t, typename... vargs_t>
		static auto assemble_to(void * place_at, void * end, vargs_t... vargs) -> ret_t *
		{
			#if 0
			if constexpr (OPERAND_SIZE != 0)
				static_assert( sizeof...(vargs) > 0 && (sizeof(decltype(vargs)) + ...) == OPERAND_SIZE, "Parameters provided to the assembler does not total to the expected operand size.");
			#endif

			if (!place_at || !end)
				return nullptr;

			std::uint8_t * current = reinterpret_cast<std::uint8_t *>(place_at);
			if (current + sizeof(op.code) >= end)
				return nullptr;

			// Place the opcode
			*reinterpret_cast<decltype(op.code)*>(current) = op.code;
			current += 4; // advance the current pointer

			// Place the args as operands for our opcode
			if constexpr (OPERAND_SIZE != 0)
			{
				if (!([&] {
					// Makes sure that when we write the operand we don't go over the end
					if ((current + sizeof(decltype(vargs))) >= end)
						return false;

					if constexpr (std::is_same_v<decltype(vargs), const char *>)
					{
						const char * s_source = vargs;
						/*+1 to make sure a space for a null terminator can be inserted*/
						while (current + 1 < end && *s_source)
						{
							*current = *s_source;
							++current;
							++s_source;
						}

						*current++ = '\0';
					}
					else
					{
						*reinterpret_cast<decltype(vargs) *>(current) = vargs; // Write the operand
						current += sizeof(decltype(vargs)); // Advance the current write buffer
					}
					
					return true;
				} () && ...))
				{
					return nullptr;
				}
			}

			return reinterpret_cast<ret_t *>(current); // Return where the current pointer is at after assembling the instruction to the buffer
		}
	};
	
	// Runs a compile time check to ensure that there are no opcodes that have the same code
	struct opcode_collision_check
	{
	private:
		template <typename current>
		consteval static auto consume() -> bool
		{
			return false;
		}

		template <typename current, typename... others> requires (sizeof...(others) != 0)
		consteval static auto consume() -> bool
		{
			// TODO: silence warning
			if constexpr (constexpr auto curr_code = current::CODE; ((curr_code == others::CODE) || ...))
				return true;
			else
				return consume<others...>();
		}

	public:
		// Returns true if there are no collisions, otherwise false.
		template <typename... opcodes>
		consteval static auto run() -> bool
		{
			return consume<opcodes...>();
		}
	};
}

namespace khuneo::vm::codes
{
	// No operation
	using op_nop = impl::define_opcode<"nop", 0, [](KHUNEO_CTX_PARAM)
	{
		KHUNEO_CTX.registers.ip.ptr += 4;
	}>;

	// Intermidiate interrupt
	//		inti <interrupt code | 1 byte> <optional data>
	// Interrupt codes:
	// 
	//		'a' = Any interrupt | inti a
	//			> Nothing significant, can be used for any purpose. eg sending this interrupt
	//			  to do things from the interrupt handler
	//			> Invokes handler? Yes
	// 
	//		'm' = Message interrupt | inti m "message\0"
	//			> Sets the r0 register to the address of the null terminated cstring message
	//			> Invokes handler? Yes
	//
	using op_inti = impl::define_opcode<"inti", 1, [](KHUNEO_CTX_PARAM)
	{
		KHUNEO_CTX.registers.interrupt_flag = reinterpret_cast<const char *>(KHUNEO_CTX.registers.ip.ptr)[4];

		// Process the interrupt request
		switch (KHUNEO_CTX.registers.interrupt_flag)
		{
			case 'a':
			{
				if (KHUNEO_CTX.interrupt_handler)
					KHUNEO_CTX.interrupt_handler(KHUNEO_CTX);

				KHUNEO_CTX.registers.ip.ptr += 5;
				break;
			}

			case 'm':
			{
				char *message = reinterpret_cast<char *>(KHUNEO_CTX.registers.ip.ptr + 5);
				KHUNEO_CTX.registers.r0.ptr = reinterpret_cast<decltype(KHUNEO_CTX.registers.r0.ptr)>(message);

				if (KHUNEO_CTX.interrupt_handler)
				{
					KHUNEO_CTX.interrupt_handler(KHUNEO_CTX);
				}
				else
				{
					if (KHUNEO_CTX.exception_handler)
						KHUNEO_CTX.exception_handler(KHUNEO_CTX, khuneo::vm::exceptions::NO_INTERRUPT_HANDLER);
				}

				// Advance the instruction pointer past the message string
				while (*message) ++message;
				KHUNEO_CTX.registers.ip.ptr = reinterpret_cast<decltype(KHUNEO_CTX.registers.ip.ptr)>(message + 1);
				break;
			}

			default:
			{
				if (KHUNEO_CTX.exception_handler)
					KHUNEO_CTX.exception_handler(KHUNEO_CTX, khuneo::vm::exceptions::INVALID_INTERRUPT_CODE);

				KHUNEO_CTX.registers.ip.iptr = -1;
				break;
			}
		}
		
		// Reset the interrupt flag
		KHUNEO_CTX.registers.interrupt_flag = 0;
	}>;
}