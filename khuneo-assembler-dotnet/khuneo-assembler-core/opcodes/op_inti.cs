using System;
using System.Collections.Generic;
using System.Text;

namespace khuneo_assembler_core.opcodes
{
    public class op_inti : opcode
    {
        public op_inti() : base("inti", 1) {}

        public override bool parse(string code)
        {
            throw new NotImplementedException();
        }
    }
}
