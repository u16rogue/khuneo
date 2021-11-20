using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace khuneo_assembler_gui
{
    internal class app_config
    {
        public const string config_path = "./config.json";

        public List<string> recent_files   = new();
        public string       khuneo_runtime = null;

        public bool save_to_file(string file_path = app_config.config_path)
        {
            try
            {
                using (StreamWriter sr = new(file_path))
                {
                    sr.Write(JsonConvert.SerializeObject(this));
                }
            }
            catch (Exception)
            {
                return false;
            }

            return true;
        }

        public static bool save_to_file(in app_config instance, string file_path = app_config.config_path)
        {
            return instance.save_to_file(file_path);
        }

        public static app_config load_from_file(string file_path = app_config.config_path)
        {
            if (!File.Exists(file_path))
            {
                new app_config().save_to_file(file_path);
            }

            try
            {
                using (StreamReader sr = new(file_path))
                {
                    return JsonConvert.DeserializeObject<app_config>(sr.ReadToEnd());
                }
            }
            catch (Exception)
            {
                return null;
            }
        }
    }
}
