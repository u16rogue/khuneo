#pragma once
#include <khuneo/defs.hpp>

namespace khuneo::impl::vm
{
	constexpr khuneo::kh_bytecode_t FLAG_OPCODE_IS_EXTENDED = 0x80;	
	
	enum class op_type : kh_bytecode_t
	{
		SYMBOL        = 0b00,
		REGISTER      = 0b01,
		INTERMIDIATE  = 0b10,
		STACK         = 0b11
	};

	enum class op_size : kh_bytecode_t
	{
		ONE   = 0b00,
		TWO   = 0b01,
		FOUR  = 0b10,
		EIGHT = 0b11
	};

	enum class op_jmp_type : kh_bytecode_t
	{
		ABSOLUTE = 0,
		RELATIVE = 1
	};

	struct op_descriptor
	{
		union
		{	
			struct
			{
				op_type destination_type : 2; // This should never be an immediate
				op_type source_type : 2;
				unsigned char reserved : 2;
				op_size source_size : 2;
			} op_set;

			struct
			{
				op_type displacement_source : 2;
				op_jmp_type jump_type : 1;
				unsigned char reserved : 3;
				op_size displacement_size : 2; // Will only be used if displacement_source is INTERMIDIATE
			} op_jmp;

			struct
			{
				unsigned char reserved : 4;

				// [opcode] [operand0], [operand1]
				unsigned char operand0 : 2;
				unsigned char operand1 : 2;
			};
		};
	};
	static_assert(sizeof(op_descriptor) == 1, "khuneo::impl::vm::op_descriptor was expected to have the size of 1 byte");	

	enum class opcodes : kh_bytecode_t
	{
		_FIRST_ITEM,
			
		INTERRUPT = 0x01,
		SET_FLAG = 0x02,

		/*
		* Define
		* 
		* Creates a new symbol
		*  
		* [opcode 1b] [32 bit xxh hash]	
		*/
		DEFINE,
	
		/*
		* Copy
		* 
		* Copies a value from source to destination
		*
		* [opcode 1b] [descriptor] [destination], [source]
		*/
		COPY,

		/*
		* Jump
		* 
		* [opcode 1b] [descriptor] [destination]
		*/
		JMP,

		_LAST_ITEM
	};
	static_assert((int)opcodes::_FIRST_ITEM == 0 && ~(int)opcodes::_LAST_ITEM & FLAG_OPCODE_IS_EXTENDED, "Either Opcode ID is invalid or an enumerator has an unintentional extended bit!"); // the last item check is not ideal.

}

namespace khuneo::vm
{

	enum class interrupts : unsigned char
	{

	};

	struct vm_register
	{
		// Value
		union
		{
			unsigned char * v_p;
		};
	};

	struct vm_context
	{
		// Why 16 (0x10)? with this we can index 2 registers in a single byte eg. COPY IP, RESULT will only use up 3 bytes
		// where we use the third byte's upper nibble to index the destination register and the lower nibble for the source register 
		static constexpr int nregisters = 0x10; 

		union
		{
			struct
			{
				vm_register ip; // instruction pointer
				vm_register result; // Result register
			};

			vm_register ireg[nregisters];
		} registers;

		static_assert(sizeof(registers) == sizeof(vm_register) * nregisters, "Virtual machine register count is invalid!");

		kh_bytecode_t * start;
		kh_bytecode_t * end;
	};

	auto vm_execute(vm_context * ctx) -> bool;
}