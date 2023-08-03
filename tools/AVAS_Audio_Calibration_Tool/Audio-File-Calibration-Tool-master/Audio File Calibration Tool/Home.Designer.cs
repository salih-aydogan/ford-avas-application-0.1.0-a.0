namespace Audio_File_Calibration_Tool
{
    partial class Home
    {
        /// <summary>
        ///Gerekli tasarımcı değişkeni.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///Kullanılan tüm kaynakları temizleyin.
        /// </summary>
        ///<param name="disposing">yönetilen kaynaklar dispose edilmeliyse doğru; aksi halde yanlış.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer üretilen kod

        /// <summary>
        /// Tasarımcı desteği için gerekli metot - bu metodun 
        ///içeriğini kod düzenleyici ile değiştirmeyin.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Home));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.clearToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.helpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.aboutToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.label1 = new System.Windows.Forms.Label();
            this.txtBox_CalibrationVersion = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.btn_open = new System.Windows.Forms.Button();
            this.txt_filePath = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.btn_generate = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.txtBox_AudioFileVersion = new System.Windows.Forms.TextBox();
            this.txt_boxShiftDenominator = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label13 = new System.Windows.Forms.Label();
            this.txt_bitsPerSample = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.txt_wavFormat = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.txt_byteRate = new System.Windows.Forms.TextBox();
            this.txt_sampleRate = new System.Windows.Forms.TextBox();
            this.txt_numChannels = new System.Windows.Forms.TextBox();
            this.lbl_version = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.txt_binPath = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.lbl_FileToolVersion = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.lbl_audioFileName = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.GripMargin = new System.Windows.Forms.Padding(2, 2, 0, 2);
            this.menuStrip1.ImageScalingSize = new System.Drawing.Size(24, 24);
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.helpToolStripMenuItem,
            this.aboutToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(1415, 33);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openToolStripMenuItem,
            this.clearToolStripMenuItem,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(54, 29);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(158, 34);
            this.openToolStripMenuItem.Text = "Open";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // clearToolStripMenuItem
            // 
            this.clearToolStripMenuItem.Name = "clearToolStripMenuItem";
            this.clearToolStripMenuItem.Size = new System.Drawing.Size(158, 34);
            this.clearToolStripMenuItem.Text = "Clear";
            this.clearToolStripMenuItem.Click += new System.EventHandler(this.clearToolStripMenuItem_Click);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(158, 34);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // helpToolStripMenuItem
            // 
            this.helpToolStripMenuItem.Name = "helpToolStripMenuItem";
            this.helpToolStripMenuItem.Size = new System.Drawing.Size(65, 29);
            this.helpToolStripMenuItem.Text = "Help";
            this.helpToolStripMenuItem.Click += new System.EventHandler(this.helpToolStripMenuItem_Click);
            // 
            // aboutToolStripMenuItem
            // 
            this.aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
            this.aboutToolStripMenuItem.Size = new System.Drawing.Size(78, 29);
            this.aboutToolStripMenuItem.Text = "About";
            this.aboutToolStripMenuItem.Click += new System.EventHandler(this.aboutToolStripMenuItem_Click);
            // 
            // dataGridView1
            // 
            this.dataGridView1.AllowUserToOrderColumns = true;
            this.dataGridView1.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.ColumnHeader;
            this.dataGridView1.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.AllHeaders;
            this.dataGridView1.BackgroundColor = System.Drawing.Color.WhiteSmoke;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.GridColor = System.Drawing.Color.SlateGray;
            this.dataGridView1.Location = new System.Drawing.Point(12, 36);
            this.dataGridView1.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.RowHeadersWidth = 62;
            this.dataGridView1.RowTemplate.Height = 28;
            this.dataGridView1.Size = new System.Drawing.Size(698, 778);
            this.dataGridView1.TabIndex = 2;
            this.dataGridView1.TabStop = false;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 34);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(191, 20);
            this.label1.TabIndex = 3;
            this.label1.Text = "Audio File Version: (XY.Z)";
            // 
            // txtBox_CalibrationVersion
            // 
            this.txtBox_CalibrationVersion.Location = new System.Drawing.Point(280, 68);
            this.txtBox_CalibrationVersion.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txtBox_CalibrationVersion.Name = "txtBox_CalibrationVersion";
            this.txtBox_CalibrationVersion.Size = new System.Drawing.Size(224, 26);
            this.txtBox_CalibrationVersion.TabIndex = 8;
            this.txtBox_CalibrationVersion.TextChanged += new System.EventHandler(this.txtBox_CalibrationVersion_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 71);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(200, 20);
            this.label3.TabIndex = 7;
            this.label3.Text = "Calibration Version: (XY.Z) ";
            // 
            // btn_open
            // 
            this.btn_open.Location = new System.Drawing.Point(144, 62);
            this.btn_open.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btn_open.Name = "btn_open";
            this.btn_open.Size = new System.Drawing.Size(424, 41);
            this.btn_open.TabIndex = 12;
            this.btn_open.Text = "Open";
            this.btn_open.UseVisualStyleBackColor = true;
            this.btn_open.Click += new System.EventHandler(this.btn_open_Click);
            // 
            // txt_filePath
            // 
            this.txt_filePath.Location = new System.Drawing.Point(144, 31);
            this.txt_filePath.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txt_filePath.Name = "txt_filePath";
            this.txt_filePath.ReadOnly = true;
            this.txt_filePath.ShortcutsEnabled = false;
            this.txt_filePath.Size = new System.Drawing.Size(424, 26);
            this.txt_filePath.TabIndex = 10;
            this.txt_filePath.WordWrap = false;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(11, 31);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(124, 20);
            this.label2.TabIndex = 9;
            this.label2.Text = "WAV File Path : ";
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.ImageScalingSize = new System.Drawing.Size(24, 24);
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(61, 4);
            // 
            // btn_generate
            // 
            this.btn_generate.Location = new System.Drawing.Point(887, 727);
            this.btn_generate.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btn_generate.Name = "btn_generate";
            this.btn_generate.Size = new System.Drawing.Size(424, 41);
            this.btn_generate.TabIndex = 14;
            this.btn_generate.Text = "Generate";
            this.btn_generate.UseVisualStyleBackColor = true;
            this.btn_generate.Click += new System.EventHandler(this.btn_generate_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.txtBox_AudioFileVersion);
            this.groupBox1.Controls.Add(this.txtBox_CalibrationVersion);
            this.groupBox1.Location = new System.Drawing.Point(743, 50);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox1.Size = new System.Drawing.Size(616, 117);
            this.groupBox1.TabIndex = 15;
            this.groupBox1.TabStop = false;
            // 
            // txtBox_AudioFileVersion
            // 
            this.txtBox_AudioFileVersion.Location = new System.Drawing.Point(280, 28);
            this.txtBox_AudioFileVersion.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txtBox_AudioFileVersion.Name = "txtBox_AudioFileVersion";
            this.txtBox_AudioFileVersion.Size = new System.Drawing.Size(224, 26);
            this.txtBox_AudioFileVersion.TabIndex = 4;
            this.txtBox_AudioFileVersion.TextChanged += new System.EventHandler(this.txtBox_AudioFileVersion_TextChanged);
            // 
            // txt_boxShiftDenominator
            // 
            this.txt_boxShiftDenominator.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.SuggestAppend;
            this.txt_boxShiftDenominator.BackColor = System.Drawing.Color.Green;
            this.txt_boxShiftDenominator.Location = new System.Drawing.Point(486, 829);
            this.txt_boxShiftDenominator.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txt_boxShiftDenominator.Name = "txt_boxShiftDenominator";
            this.txt_boxShiftDenominator.Size = new System.Drawing.Size(224, 26);
            this.txt_boxShiftDenominator.TabIndex = 21;
            this.txt_boxShiftDenominator.Text = "100";
            this.txt_boxShiftDenominator.TextChanged += new System.EventHandler(this.txt_boxShiftDenominator_TextChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(297, 832);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(183, 20);
            this.label5.TabIndex = 20;
            this.label5.Text = "Freq Shift Denominator :";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label13);
            this.groupBox2.Controls.Add(this.txt_bitsPerSample);
            this.groupBox2.Controls.Add(this.label12);
            this.groupBox2.Controls.Add(this.txt_wavFormat);
            this.groupBox2.Controls.Add(this.label11);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.label9);
            this.groupBox2.Controls.Add(this.txt_byteRate);
            this.groupBox2.Controls.Add(this.txt_sampleRate);
            this.groupBox2.Controls.Add(this.txt_numChannels);
            this.groupBox2.Controls.Add(this.btn_open);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.txt_filePath);
            this.groupBox2.Location = new System.Drawing.Point(743, 171);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox2.Size = new System.Drawing.Size(616, 530);
            this.groupBox2.TabIndex = 16;
            this.groupBox2.TabStop = false;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(-1, 350);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(130, 20);
            this.label13.TabIndex = 33;
            this.label13.Text = "Bits Per Sample :";
            // 
            // txt_bitsPerSample
            // 
            this.txt_bitsPerSample.Location = new System.Drawing.Point(144, 347);
            this.txt_bitsPerSample.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txt_bitsPerSample.Name = "txt_bitsPerSample";
            this.txt_bitsPerSample.ReadOnly = true;
            this.txt_bitsPerSample.ShortcutsEnabled = false;
            this.txt_bitsPerSample.Size = new System.Drawing.Size(424, 26);
            this.txt_bitsPerSample.TabIndex = 32;
            this.txt_bitsPerSample.WordWrap = false;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(-1, 424);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(132, 80);
            this.label12.TabIndex = 31;
            this.label12.Text = "Byte Rate : \r\n(SampleRate * \r\nNumChannels * \r\nBitsPerSample/8)";
            // 
            // txt_wavFormat
            // 
            this.txt_wavFormat.Location = new System.Drawing.Point(144, 123);
            this.txt_wavFormat.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txt_wavFormat.Name = "txt_wavFormat";
            this.txt_wavFormat.ReadOnly = true;
            this.txt_wavFormat.ShortcutsEnabled = false;
            this.txt_wavFormat.Size = new System.Drawing.Size(424, 26);
            this.txt_wavFormat.TabIndex = 29;
            this.txt_wavFormat.WordWrap = false;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(-1, 290);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(110, 20);
            this.label11.TabIndex = 30;
            this.label11.Text = "Sample Rate: ";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(-1, 194);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(139, 60);
            this.label10.TabIndex = 28;
            this.label10.Text = "Num of Channels :\r\n(1 => Mono)\r\n(2 => Stereo)";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(3, 123);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(113, 40);
            this.label9.TabIndex = 27;
            this.label9.Text = "WAV Format : \r\n(1 => PCM)";
            // 
            // txt_byteRate
            // 
            this.txt_byteRate.Location = new System.Drawing.Point(144, 424);
            this.txt_byteRate.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txt_byteRate.Name = "txt_byteRate";
            this.txt_byteRate.ReadOnly = true;
            this.txt_byteRate.ShortcutsEnabled = false;
            this.txt_byteRate.Size = new System.Drawing.Size(424, 26);
            this.txt_byteRate.TabIndex = 26;
            this.txt_byteRate.WordWrap = false;
            // 
            // txt_sampleRate
            // 
            this.txt_sampleRate.Location = new System.Drawing.Point(144, 284);
            this.txt_sampleRate.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txt_sampleRate.Name = "txt_sampleRate";
            this.txt_sampleRate.ReadOnly = true;
            this.txt_sampleRate.ShortcutsEnabled = false;
            this.txt_sampleRate.Size = new System.Drawing.Size(424, 26);
            this.txt_sampleRate.TabIndex = 25;
            this.txt_sampleRate.WordWrap = false;
            // 
            // txt_numChannels
            // 
            this.txt_numChannels.Location = new System.Drawing.Point(144, 194);
            this.txt_numChannels.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txt_numChannels.Name = "txt_numChannels";
            this.txt_numChannels.ReadOnly = true;
            this.txt_numChannels.ShortcutsEnabled = false;
            this.txt_numChannels.Size = new System.Drawing.Size(424, 26);
            this.txt_numChannels.TabIndex = 24;
            this.txt_numChannels.WordWrap = false;
            // 
            // lbl_version
            // 
            this.lbl_version.AutoSize = true;
            this.lbl_version.Location = new System.Drawing.Point(979, 812);
            this.lbl_version.Name = "lbl_version";
            this.lbl_version.Size = new System.Drawing.Size(0, 20);
            this.lbl_version.TabIndex = 17;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(11, 22);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(133, 20);
            this.label4.TabIndex = 18;
            this.label4.Text = "Audio File Name :";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.txt_binPath);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Controls.Add(this.lbl_FileToolVersion);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.lbl_audioFileName);
            this.groupBox3.Controls.Add(this.label4);
            this.groupBox3.ForeColor = System.Drawing.SystemColors.ControlText;
            this.groupBox3.ImeMode = System.Windows.Forms.ImeMode.Hiragana;
            this.groupBox3.Location = new System.Drawing.Point(743, 788);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.groupBox3.Size = new System.Drawing.Size(616, 164);
            this.groupBox3.TabIndex = 19;
            this.groupBox3.TabStop = false;
            this.groupBox3.Visible = false;
            // 
            // txt_binPath
            // 
            this.txt_binPath.Location = new System.Drawing.Point(176, 107);
            this.txt_binPath.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txt_binPath.Name = "txt_binPath";
            this.txt_binPath.ReadOnly = true;
            this.txt_binPath.ShortcutsEnabled = false;
            this.txt_binPath.Size = new System.Drawing.Size(360, 26);
            this.txt_binPath.TabIndex = 23;
            this.txt_binPath.WordWrap = false;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(11, 107);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(114, 20);
            this.label7.TabIndex = 22;
            this.label7.Text = "BIN File Path : ";
            // 
            // lbl_FileToolVersion
            // 
            this.lbl_FileToolVersion.AutoSize = true;
            this.lbl_FileToolVersion.Location = new System.Drawing.Point(201, 64);
            this.lbl_FileToolVersion.Name = "lbl_FileToolVersion";
            this.lbl_FileToolVersion.Size = new System.Drawing.Size(0, 20);
            this.lbl_FileToolVersion.TabIndex = 21;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(11, 64);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(184, 20);
            this.label6.TabIndex = 20;
            this.label6.Text = "Calibration Tool Version :";
            // 
            // lbl_audioFileName
            // 
            this.lbl_audioFileName.AutoSize = true;
            this.lbl_audioFileName.Location = new System.Drawing.Point(150, 23);
            this.lbl_audioFileName.Name = "lbl_audioFileName";
            this.lbl_audioFileName.Size = new System.Drawing.Size(0, 20);
            this.lbl_audioFileName.TabIndex = 19;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(18, 956);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(875, 140);
            this.label8.TabIndex = 22;
            this.label8.Text = resources.GetString("label8.Text");
            // 
            // Home
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1415, 1119);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.txt_boxShiftDenominator);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.lbl_version);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.dataGridView1);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.btn_generate);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Name = "Home";
            this.Text = "Daiichi AVAS Audio Calibration Tool";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem helpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem aboutToolStripMenuItem;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtBox_CalibrationVersion;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button btn_open;
        private System.Windows.Forms.TextBox txt_filePath;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.Button btn_generate;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.ToolStripMenuItem clearToolStripMenuItem;
        private System.Windows.Forms.Label lbl_version;
        private System.Windows.Forms.TextBox txtBox_AudioFileVersion;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label lbl_FileToolVersion;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label lbl_audioFileName;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox txt_boxShiftDenominator;
        private System.Windows.Forms.TextBox txt_binPath;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox txt_byteRate;
        private System.Windows.Forms.TextBox txt_sampleRate;
        private System.Windows.Forms.TextBox txt_numChannels;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox txt_wavFormat;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox txt_bitsPerSample;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label13;
    }
}

