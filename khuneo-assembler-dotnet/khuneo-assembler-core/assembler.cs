using System;
using System.Collections.Generic;
using System.Text;

namespace khuneo_assembler_core
{
    public class assembler
    {
        private opcode.logger_delegate logger;

        public assembler(opcode.logger_delegate logger_)
        {
            this.logger = logger_;
        }

        public List<byte>? assemble(string code)
        {
            List<byte> byte_code = new() {  };

            int line_num = 0;
            foreach (string line in code.Split('\n'))
            {
                ++line_num;
                foreach (opcode op in assembler.registered_opcodes)
                {
                    if (line.StartsWith(op.mnenomic))
                    {
                        byte_code.AddRange(BitConverter.GetBytes(op.code));
                        if (op.parse(ref byte_code, line.Substring(op.mnenomic.Length), this.logger))
                        {
                            goto LBL_NEXT_LINE;
                        }
                        else
                        {
                            logger($"Failed to assemble at line {line_num}.");
                            return null;
                        }
                    }
                }

                LBL_NEXT_LINE:;
            }

            return byte_code;
        }

        public static List<opcode> registered_opcodes = new()
        {
            new opcodes.op_nop(),
            new opcodes.op_inti()
        };
    }
}
