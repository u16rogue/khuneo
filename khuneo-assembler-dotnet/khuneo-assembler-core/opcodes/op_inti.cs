using System;
using System.Collections.Generic;
using System.Text;
using khuneo_assembler_core.extensions;

namespace khuneo_assembler_core.opcodes
{
    public class op_inti : opcode
    {
        public op_inti() : base("inti", 1) {}

        private bool match_interrupt(in string code, string operand, ref string extra)
        {
            string match = " " + operand;
            if (!code.StartsWith(match))
                return false;

            extra = code.Substring(match.Length + 1);
            return true;
        }

        public override bool parse(ref List<byte> byte_buffer, string code, logging.cb_delegate logger)
        {
            logger("Parsing interrupt...", logging.type.MESSAGE);

            if (code.IsNullOrWhitespace())
            {
                logger("WARNING: No operand found, assuming interrupt code is \"any\". Encoded any interrupt.", logging.type.WARNING);
                byte_buffer.Add(Convert.ToByte('a'));
                return true;
            }

            string extra = "";
            if (match_interrupt(code, "message", ref extra))
            {
                if (extra.IsNullOrWhitespace())
                {
                    logger("Could not encode message interrupt without a message", logging.type.ERROR);
                    return false;
                }

                // Add the interrupt code operand
                byte_buffer.Add(Convert.ToByte('m'));

                // Add the rest of the string
                byte_buffer.AddRange(new UTF8Encoding().GetBytes(extra));
                byte_buffer.Add(0); // add the null terminator
                logger("Encoded message interrupt", logging.type.SUCCESS);
            }
            else if (match_interrupt(code, "any", ref extra))
            {
                byte_buffer.Add(Convert.ToByte('a'));
                logger("Encoded any interrupt", logging.type.SUCCESS);
                return true;
            }
            else
            {
                logger("Invalid interrupt opcode!", logging.type.ERROR);
                return false;
            }
            
            return true;
        }
    }
}
