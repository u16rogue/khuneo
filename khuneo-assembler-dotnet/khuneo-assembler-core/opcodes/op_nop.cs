using System;
using System.Collections.Generic;
using System.Text;

namespace khuneo_assembler_core.opcodes
{
    public class op_nop : opcode
    {
        public op_nop() : base("nop", 0) {}

        public override bool parse(string code)
        {
            throw new NotImplementedException();
        }
    }
}
