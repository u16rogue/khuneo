using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace khuneo_assembler_gui
{
    internal static class Program
    {
        public static app_config config;

        public static form_main  form_main;
        public static string     file_source = null;
        public static string     runner      = null;

        public static bool load_source_file(string file_path = null)
        {
            try
            {
                if (file_path != null)
                    file_source = file_path;

                if (!File.Exists(file_source))
                    return false;

                using (StreamReader sr = new(file_source))
                {
                    form_main.rtb_code.Text = sr.ReadToEnd();
                }

                form_main.update_title();
                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }

        public static bool save_source_file(string file_path = null)
        {
            try
            {
                if (file_path != null)
                    file_source = file_path;

                if (file_source == null)
                    return false;

                using (StreamWriter sw = new(file_source))
                {
                    sw.Write(form_main.rtb_code.Text);
                }

                form_main.update_title();
                return true;
            }
            catch (Exception)
            {
                return false;
            }
        }

        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Program.config = app_config.load_from_file();
            Program.form_main = new ();
            Application.Run(Program.form_main);
        }
    }
}
