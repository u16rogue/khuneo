using System;
using System.Collections.Generic;
using System.Text;

namespace khuneo_assembler_core.opcodes
{
    public class op_nop : opcode
    {
        public op_nop() : base("nop", 0) {}

        public override bool parse(ref List<byte> byte_buffer, string code, logging.cb_delegate logger)
        {
            if (code.Length != 0)
            {
                logger("The nop opcode does not take operands.", logging.type.ERROR);
                return false;
            }

            return true;
        }
    }
}
