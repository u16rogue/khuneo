using System;
using System.Collections.Generic;
using System.Text;

namespace khuneo_assembler_core
{
    public static class hash
    {
        public static UInt32 fnv32(string str)
        {
            UInt32 hash = 0x811c9dc5;
            foreach (char c in str)
                hash = (hash * 0x01000193) ^ c;

            return hash;
        }
    }
}
