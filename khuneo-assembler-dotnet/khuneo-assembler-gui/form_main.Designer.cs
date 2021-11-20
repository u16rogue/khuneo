namespace khuneo_assembler_gui
{
    partial class form_main
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(form_main));
            this.rtb_code = new System.Windows.Forms.RichTextBox();
            this.ts_top = new System.Windows.Forms.ToolStrip();
            this.tsdb_file = new System.Windows.Forms.ToolStripDropDownButton();
            this.tsmi_new = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmi_open = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmi_save = new System.Windows.Forms.ToolStripMenuItem();
            this.tsmi_recent = new System.Windows.Forms.ToolStripMenuItem();
            this.btn_assemble = new System.Windows.Forms.ToolStripButton();
            this.btn_run = new System.Windows.Forms.ToolStripButton();
            this.ofd = new System.Windows.Forms.OpenFileDialog();
            this.sfd = new System.Windows.Forms.SaveFileDialog();
            this.ts_top.SuspendLayout();
            this.SuspendLayout();
            // 
            // rtb_code
            // 
            this.rtb_code.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.rtb_code.DetectUrls = false;
            this.rtb_code.Font = new System.Drawing.Font("Cascadia Code SemiBold", 15.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.rtb_code.Location = new System.Drawing.Point(0, 28);
            this.rtb_code.Name = "rtb_code";
            this.rtb_code.Size = new System.Drawing.Size(800, 491);
            this.rtb_code.TabIndex = 0;
            this.rtb_code.Text = "";
            // 
            // ts_top
            // 
            this.ts_top.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsdb_file,
            this.btn_assemble,
            this.btn_run});
            this.ts_top.Location = new System.Drawing.Point(0, 0);
            this.ts_top.Name = "ts_top";
            this.ts_top.Size = new System.Drawing.Size(800, 25);
            this.ts_top.TabIndex = 1;
            this.ts_top.Text = "toolStrip1";
            // 
            // tsdb_file
            // 
            this.tsdb_file.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.tsdb_file.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tsmi_new,
            this.tsmi_open,
            this.tsmi_save,
            this.tsmi_recent});
            this.tsdb_file.Image = ((System.Drawing.Image)(resources.GetObject("tsdb_file.Image")));
            this.tsdb_file.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.tsdb_file.Name = "tsdb_file";
            this.tsdb_file.Size = new System.Drawing.Size(38, 22);
            this.tsdb_file.Text = "File";
            // 
            // tsmi_new
            // 
            this.tsmi_new.Name = "tsmi_new";
            this.tsmi_new.Size = new System.Drawing.Size(180, 22);
            this.tsmi_new.Text = "New";
            this.tsmi_new.Click += new System.EventHandler(this.tsmi_new_Click);
            // 
            // tsmi_open
            // 
            this.tsmi_open.Name = "tsmi_open";
            this.tsmi_open.Size = new System.Drawing.Size(180, 22);
            this.tsmi_open.Text = "Open";
            this.tsmi_open.Click += new System.EventHandler(this.tsmi_open_Click);
            // 
            // tsmi_save
            // 
            this.tsmi_save.Name = "tsmi_save";
            this.tsmi_save.Size = new System.Drawing.Size(180, 22);
            this.tsmi_save.Text = "Save";
            this.tsmi_save.Click += new System.EventHandler(this.tsmi_save_Click);
            // 
            // tsmi_recent
            // 
            this.tsmi_recent.Name = "tsmi_recent";
            this.tsmi_recent.Size = new System.Drawing.Size(180, 22);
            this.tsmi_recent.Text = "Recent";
            // 
            // btn_assemble
            // 
            this.btn_assemble.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btn_assemble.Image = ((System.Drawing.Image)(resources.GetObject("btn_assemble.Image")));
            this.btn_assemble.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btn_assemble.Name = "btn_assemble";
            this.btn_assemble.Size = new System.Drawing.Size(62, 22);
            this.btn_assemble.Text = "Assemble";
            this.btn_assemble.Click += new System.EventHandler(this.btn_assemble_Click);
            // 
            // btn_run
            // 
            this.btn_run.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btn_run.Image = ((System.Drawing.Image)(resources.GetObject("btn_run.Image")));
            this.btn_run.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btn_run.Name = "btn_run";
            this.btn_run.Size = new System.Drawing.Size(32, 22);
            this.btn_run.Text = "Run";
            // 
            // ofd
            // 
            this.ofd.FileName = "Untitled.kel";
            this.ofd.Filter = "Khuneo Assembly|*.kel";
            // 
            // sfd
            // 
            this.sfd.FileName = "Untitled.kel";
            this.sfd.Filter = "Khuneo Assembly|*.kel";
            // 
            // form_main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 519);
            this.Controls.Add(this.ts_top);
            this.Controls.Add(this.rtb_code);
            this.Font = new System.Drawing.Font("Cascadia Code", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Name = "form_main";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "khuneo assembler";
            this.Load += new System.EventHandler(this.on_load);
            this.ts_top.ResumeLayout(false);
            this.ts_top.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.ToolStrip ts_top;
        private System.Windows.Forms.ToolStripDropDownButton tsdb_file;
        private System.Windows.Forms.ToolStripMenuItem tsmi_new;
        private System.Windows.Forms.ToolStripMenuItem tsmi_open;
        private System.Windows.Forms.ToolStripMenuItem tsmi_save;
        private System.Windows.Forms.ToolStripMenuItem tsmi_recent;
        private System.Windows.Forms.ToolStripButton btn_assemble;
        private System.Windows.Forms.ToolStripButton btn_run;
        private System.Windows.Forms.OpenFileDialog ofd;
        private System.Windows.Forms.SaveFileDialog sfd;
        internal System.Windows.Forms.RichTextBox rtb_code;
    }
}

