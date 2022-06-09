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

	enum class op_define_mode : kh_bytecode_t
	{
		HASH,
		STRING,
	};

	enum class op_define_level : kh_bytecode_t
	{
		GLOBAL,
		SCOPE
	};

	enum class op_define_type : kh_bytecode_t
	{
		SYMBOL,
		TYPE
	};

	struct op_descriptor
	{
		union
		{	
			struct
			{
				op_type       destination_type : 2; // This should never be an immediate
				op_type       source_type      : 2;
				unsigned char reserved         : 2;
				op_size       source_size      : 2;
			} op_copy;

			struct
			{
				op_type       displacement_source : 2;
				op_jmp_type   jump_type           : 1;
				unsigned char reserved            : 3;
				op_size       displacement_size   : 2; // Will only be used if displacement_source is INTERMIDIATE
			} op_jmp;

			struct
			{
				op_define_mode  mode               : 1;
				op_define_level level              : 1;
				op_define_type  type               : 1;
				bool            load_to_result_reg : 1; // After defining load it to the VM's result register
				unsigned char   reserved           : 4;
			} op_define;

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
		* [opcode 1b] [32 bit xxh hash / null terminated string]	
		*/
		DEFINE,

		/*
		* Set Type
		*
		* Sets the type of a symbol
		* 
		* [opcode] [descriptor] [symbol hash] [type hash]
		*/
		SET_TYPE,
	
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
		* Changes the flow of execution unconditionally.
		* [opcode 1b] [descriptor] [destination]
		*/
		JMP,

		/*
		* Jump Next
		* Changes the flow of execution unconditionally after executing the next instruction.
		* by setting the JN flag to 1 ~~~~~~
		* NOTE: Displacement is estimated at the site of execution!
		* [opcode 1b] [descriptor] [destination]
		*/
		JMP_NEXT,

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
			unsigned char   v_u8;
			unsigned int    v_u32;
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