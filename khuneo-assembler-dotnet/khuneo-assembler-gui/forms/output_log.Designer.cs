namespace khuneo_assembler_gui.forms
{
    partial class output_log
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
            this.rtb_log = new System.Windows.Forms.RichTextBox();
            this.SuspendLayout();
            // 
            // rtb_log
            // 
            this.rtb_log.Dock = System.Windows.Forms.DockStyle.Fill;
            this.rtb_log.Font = new System.Drawing.Font("Cascadia Code SemiBold", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.rtb_log.Location = new System.Drawing.Point(0, 0);
            this.rtb_log.Name = "rtb_log";
            this.rtb_log.Size = new System.Drawing.Size(547, 464);
            this.rtb_log.TabIndex = 0;
            this.rtb_log.Text = "output log";
            // 
            // output_log
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(547, 464);
            this.Controls.Add(this.rtb_log);
            this.Name = "output_log";
            this.Text = "output_log";
            this.TopMost = true;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.output_log_FormClosing);
            this.ResumeLayout(false);

        }

        #endregion

        internal System.Windows.Forms.RichTextBox rtb_log;
    }
}