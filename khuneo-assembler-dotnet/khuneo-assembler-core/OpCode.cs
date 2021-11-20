using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace khuneo_assembler_core
{
    public class opcode
    {
        public opcode(string mnenomic, int operand_size)
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
        public static int ParseFromSourceFile(string filepath)
        {
            if (!File.Exists(filepath))
                return -1;

            int new_opcodes = 0;

            string source_code;
            using (StreamReader sr = new(filepath))
            {
                source_code = sr.ReadToEnd();
                sr.Close();
            }



            return new_opcodes;
        }

        /// <summary>
        /// Predefined opcodes
        /// </summary>
        static List<opcode> opcodes = new()
        {
            new opcode("nop",  0),
            new opcode("inti", 1)
        };
    }
}
