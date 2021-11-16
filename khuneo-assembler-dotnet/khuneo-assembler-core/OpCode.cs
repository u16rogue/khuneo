using System;
using System.Collections.Generic;
using System.Text;

namespace khuneo_assembler_core
{
    public class OpCode
    {
        public OpCode(string mnenomic, int operand_size)
        {
            this.mnenomic = mnenomic;
            this.operand_size = operand_size;

            // FNV1 hash
            foreach (char c in mnenomic)
                this.code = (this.code * 0x01000193) ^ c;
        }

        public readonly string mnenomic     = string.Empty;
        public readonly UInt32 code         = 0x811c9dc5;
        public readonly int    operand_size = 0;

        /// <summary>
        /// Parses a C++ source file for impl::define_opcode<> defintions and automatically imports them into the opcodes list
        /// </summary>
        public static void ParseFromSourceFile(string filepath)
        {

        }

        /// <summary>
        /// Predefined opcodes
        /// </summary>
        static List<OpCode> opcodes = new()
        {
            new OpCode("nop",  0),
            new OpCode("inti", 1)
        };
    }
}
