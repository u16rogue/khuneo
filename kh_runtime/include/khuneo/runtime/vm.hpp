#pragma once
#include <khuneo/defs.hpp>

namespace khuneo::impl::vm
{
	constexpr khuneo::kh_bytecode_t FLAG_OPCODE_IS_EXTENDED = 0x80;	

	struct op_descriptor
	{
		union
		{	
			struct
			{
				unsigned char reserved : 4;
				unsigned char operand_size1 : 2;
				unsigned char operand_size0 : 2;
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

		_LAST_ITEM
	};
	static_assert((int)opcodes::_FIRST_ITEM == 0 && ~(int)opcodes::_LAST_ITEM & FLAG_OPCODE_IS_EXTENDED, "Either Opcode ID is invalid or an enumerator has an unintentional extended bit!"); // the last item check is not ideal.

}

namespace khuneo::vm
{

	enum class interrupts : unsigned char
	{

	};

	struct vm_context
	{
		kh_bytecode_t * start;
		kh_bytecode_t * current;
		kh_bytecode_t * end;
	};

	auto vm_execute(vm_context * ctx) -> bool;
}