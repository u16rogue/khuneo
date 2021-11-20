using System;
using System.Collections.Generic;
using System.Text;

namespace khuneo_assembler_core
{
    public class assembler
    {
        public assembler()
        {

        }

        public static List<opcode> registered_opcodes = new()
        {
            new opcodes.op_nop(),
            new opcodes.op_inti()
        };
    }
}
