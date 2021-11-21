using System;
using khuneo_assembler_core;
using System.Windows.Forms;
using System.IO;
using System.Drawing;
using System.Diagnostics;
using System.Threading;
using khuneo_assembler_gui.extensions;

namespace khuneo_assembler_gui.forms
{
    public partial class form_main : Form
    {
        public form_main()
        {
            InitializeComponent();
        }

        public void update_title()
        {
            this.Text = "khuneo assembler - " + (Program.file_source != null ? Program.file_source : "Untitled (*.kel)");
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
            Program.form_log.Show();
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

        private string run_assembler()
        {
            Program.form_log.Show();

            var asm = new assembler((string msg, logging.type type) =>
            {
                Color col = Program.form_log.rtb_log.ForeColor;

                switch (type)
                {
                    case logging.type.MESSAGE:
                        col = Color.DarkGray;
                        break;
                    case logging.type.WARNING:
                        col = Color.DarkOrange;
                        break;
                    case logging.type.ERROR:
                        col = Color.Red;
                        break;
                    case logging.type.SUCCESS:
                        col = Color.DarkGreen;
                        break;
                }

                Program.form_log.rtb_log.AppendColoredText("\n" + msg, col);
            });

            var res = asm.assemble(rtb_code.Text.Split('\n'));
            if (res == null)
            {
                MessageBox.Show("Failed to assemble kel assembly file to bun bytecode", "Assembler failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return null;
            }

            string out_file = Program.file_source.Replace(".kel", ".bun");
            File.WriteAllBytes(out_file, res.ToArray());

            return out_file;
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
            this.run_assembler();
        }

        private void btn_run_Click(object sender, EventArgs e)
        {
            string out_file = this.run_assembler();
            if (out_file == null)
                return;

            if (Program.config.khuneo_runtime == null)
            {
                OpenFileDialog _ofd = new();
                _ofd.Filter = "khuneo runtime|*.exe";
                _ofd.Title = "Path to Khuneo Runtime Executable";
                _ofd.FileName = "khuneo-runtime.exe";
                if (_ofd.ShowDialog() != DialogResult.OK)
                    return;
                Program.config.khuneo_runtime = _ofd.FileName;
                Program.config.save_to_file();
            }

            new Thread(() =>
            {
                Process runtime = new();
                runtime.StartInfo.FileName = Program.config.khuneo_runtime;
                runtime.StartInfo.Arguments = out_file;
                runtime.StartInfo.UseShellExecute = false;
                runtime.StartInfo.RedirectStandardOutput = true;
                runtime.OutputDataReceived += (s, e) => Program.form_log.rtb_log.Invoke(new Action(() => Program.form_log.rtb_log.AppendColoredText("\n" + e.Data, Color.DarkBlue) ));
                runtime.Start();
                runtime.BeginOutputReadLine();
                runtime.WaitForExit();
            }).Start();
        }
    }
}
