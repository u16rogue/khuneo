using System;
using System.Collections.Generic;
using System.Text;

namespace khuneo_assembler_core
{
    public class logging
    {
        public enum type
        {
            MESSAGE,
            WARNING,
            ERROR,
            SUCCESS
        };

        public delegate void cb_delegate(string message, type type);
    }
}
