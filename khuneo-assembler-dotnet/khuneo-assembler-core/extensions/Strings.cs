using System;
using System.Collections.Generic;
using System.Text;

namespace khuneo_assembler_core.extensions
{
    public static class Strings
    {
        public static bool IsNullOrWhitespace(this string instance)
        {
            return string.IsNullOrWhiteSpace(instance);
        }
    }
}
