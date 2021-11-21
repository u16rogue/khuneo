using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace khuneo_assembler_gui.extensions
{
    public static class RichTextBox
    {
        public static void AppendColoredText(this System.Windows.Forms.RichTextBox instance, string text, Color color)
        {
            instance.SelectionStart = instance.TextLength;
            instance.SelectionLength = 0;
            instance.SelectionColor = color;
            instance.AppendText(text);
            instance.SelectionColor = instance.ForeColor;
        }
    }
}
