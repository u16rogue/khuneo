using System;
using khuneo_assembler_core;
using System.Windows.Forms;
using System.IO;

namespace khuneo_assembler_gui
{
    public partial class form_main : Form
    {
        public form_main()
        {
            InitializeComponent();
        }

        public void update_title()
        {
            this.Text = "khuneo assember - " + (Program.file_source != null ? Program.file_source : "Untitled (*.kel)");
        }

        private void on_load(object sender, EventArgs e)
        {
            // Auto load the most recent file
            if (Program.config.recent_files.Count != 0)
            {
                Program.load_source_file(Program.config.recent_files[0]);
            }
            else
            {
                this.update_title();
            }

            this.update_recent_list();
        }

        private void update_recent_list()
        {
            tsmi_recent.DropDownItems.Clear();
            foreach (var file_path in Program.config.recent_files)
            {
                tsmi_recent.DropDownItems.Add(new ToolStripMenuItem(file_path, null, (object s, EventArgs e) =>
                {
                    ToolStripMenuItem sender = (ToolStripMenuItem)s;
                    Program.load_source_file(sender.Text);
                }));
            }

            tsmi_recent.DropDownItems.Add(new ToolStripMenuItem("Clear Recent", null, (object s, EventArgs e) =>
            {
                ToolStripMenuItem sender = (ToolStripMenuItem)s;
                Program.config.recent_files.Clear();
                Program.config.save_to_file();
                this.update_recent_list();
            }));
        }

        private void tsmi_new_Click(object sender, EventArgs e)
        {
            Program.file_source = null;
            rtb_code.Text = string.Empty;
            this.update_title();
        }

        private void tsmi_open_Click(object sender, EventArgs e)
        {
            if (ofd.ShowDialog() != DialogResult.OK || !File.Exists(ofd.FileName))
                return;

            if (Program.load_source_file(ofd.FileName))
            {
                if (!Program.config.recent_files.Contains(ofd.FileName))
                {
                    Program.config.recent_files.Insert(0, ofd.FileName);
                    Program.config.save_to_file();
                    this.update_recent_list();
                }
            }
        }

        private void tsmi_save_Click(object sender, EventArgs e)
        {
            if (Program.file_source == null)
            {
                if (sfd.ShowDialog() != DialogResult.OK)
                    return;

                if (Program.save_source_file(sfd.FileName))
                {
                    Program.config.recent_files.Insert(0, sfd.FileName);
                    Program.config.save_to_file();
                    this.update_recent_list();
                }
            }

            Program.save_source_file();
        }

        private void btn_assemble_Click(object sender, EventArgs e)
        {
            var asm = new assembler((string msg) => { Console.WriteLine(msg); });
            var res = asm.assemble(rtb_code.Text);
            if (res == null)
            {
                MessageBox.Show("Failed to assemble kel assembly file to bun bytecode", "Assembler failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            File.WriteAllBytes(Program.file_source.Replace(".kel", ".bun"), res.ToArray());
        }
    }
}
