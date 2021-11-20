using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace khuneo_assembler_gui.forms
{
    public partial class output_log : Form
    {
        public output_log()
        {
            InitializeComponent();
        }

        private void output_log_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.Hide();
            e.Cancel = true;
        }
    }
}
