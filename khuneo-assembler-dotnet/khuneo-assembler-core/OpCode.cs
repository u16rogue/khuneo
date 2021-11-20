using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace khuneo_assembler_core
{
    public abstract class opcode
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

        public abstract bool parse(ref List<byte> byte_buffer, string code, logging.cb_delegate logger);
    }
}
