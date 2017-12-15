namespace TimeTracker
{
    partial class TimeTracker
    {
        /// <summary>
        /// 必要なデザイナー変数です。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 使用中のリソースをすべてクリーンアップします。
        /// </summary>
        /// <param name="disposing">マネージ リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows フォーム デザイナーで生成されたコード

        /// <summary>
        /// デザイナー サポートに必要なメソッドです。このメソッドの内容を
        /// コード エディターで変更しないでください。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.TimerInterval = new System.Windows.Forms.Timer(this.components);
            this.Menu_TimeTracker = new System.Windows.Forms.MenuStrip();
            this.ファイルFToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_TimeTracker_Exit = new System.Windows.Forms.ToolStripMenuItem();
            this.表示ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_TimeTracker_TopMost = new System.Windows.Forms.ToolStripMenuItem();
            this.ListView_TimeTracker = new System.Windows.Forms.ListView();
            this.Label_NowRunTimeValue = new System.Windows.Forms.Label();
            this.NotifyIcon = new System.Windows.Forms.NotifyIcon(this.components);
            this.Label_NowRunTime = new System.Windows.Forms.Label();
            this.ToolStripMenuItem_DisplayCsvFolder = new System.Windows.Forms.ToolStripMenuItem();
            this.ToolStripMenuItem_OutputCsv = new System.Windows.Forms.ToolStripMenuItem();
            this.Menu_TimeTracker.SuspendLayout();
            this.SuspendLayout();
            // 
            // TimerInterval
            // 
            this.TimerInterval.Enabled = true;
            this.TimerInterval.Interval = 500;
            this.TimerInterval.Tick += new System.EventHandler(this.TimerInterval_Tick);
            // 
            // Menu_TimeTracker
            // 
            this.Menu_TimeTracker.BackColor = System.Drawing.SystemColors.MenuBar;
            this.Menu_TimeTracker.ImageScalingSize = new System.Drawing.Size(24, 24);
            this.Menu_TimeTracker.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ファイルFToolStripMenuItem,
            this.表示ToolStripMenuItem});
            this.Menu_TimeTracker.Location = new System.Drawing.Point(0, 0);
            this.Menu_TimeTracker.Name = "Menu_TimeTracker";
            this.Menu_TimeTracker.Size = new System.Drawing.Size(678, 33);
            this.Menu_TimeTracker.TabIndex = 1;
            this.Menu_TimeTracker.Text = "Menu_TimeTracker";
            // 
            // ファイルFToolStripMenuItem
            // 
            this.ファイルFToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_OutputCsv,
            this.ToolStripMenuItem_TimeTracker_Exit});
            this.ファイルFToolStripMenuItem.Name = "ファイルFToolStripMenuItem";
            this.ファイルFToolStripMenuItem.Size = new System.Drawing.Size(94, 29);
            this.ファイルFToolStripMenuItem.Text = "ファイル(&F)";
            // 
            // ToolStripMenuItem_TimeTracker_Exit
            // 
            this.ToolStripMenuItem_TimeTracker_Exit.Name = "ToolStripMenuItem_TimeTracker_Exit";
            this.ToolStripMenuItem_TimeTracker_Exit.Size = new System.Drawing.Size(210, 30);
            this.ToolStripMenuItem_TimeTracker_Exit.Text = "終了";
            this.ToolStripMenuItem_TimeTracker_Exit.Click += new System.EventHandler(this.ToolStripMenuItem_TimeTracker_Exit_Click);
            // 
            // 表示ToolStripMenuItem
            // 
            this.表示ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ToolStripMenuItem_DisplayCsvFolder,
            this.ToolStripMenuItem_TimeTracker_TopMost});
            this.表示ToolStripMenuItem.Name = "表示ToolStripMenuItem";
            this.表示ToolStripMenuItem.Size = new System.Drawing.Size(81, 29);
            this.表示ToolStripMenuItem.Text = "表示(&V)";
            // 
            // ToolStripMenuItem_TimeTracker_TopMost
            // 
            this.ToolStripMenuItem_TimeTracker_TopMost.Checked = true;
            this.ToolStripMenuItem_TimeTracker_TopMost.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ToolStripMenuItem_TimeTracker_TopMost.Name = "ToolStripMenuItem_TimeTracker_TopMost";
            this.ToolStripMenuItem_TimeTracker_TopMost.Size = new System.Drawing.Size(232, 30);
            this.ToolStripMenuItem_TimeTracker_TopMost.Text = "常に最前面に表示";
            this.ToolStripMenuItem_TimeTracker_TopMost.Click += new System.EventHandler(this.ToolStripMenuItem_TimeTracker_TopMost_Click);
            // 
            // ListView_TimeTracker
            // 
            this.ListView_TimeTracker.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.ListView_TimeTracker.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.ListView_TimeTracker.Font = new System.Drawing.Font("Yu Gothic UI", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.ListView_TimeTracker.GridLines = true;
            this.ListView_TimeTracker.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.ListView_TimeTracker.Location = new System.Drawing.Point(5, 65);
            this.ListView_TimeTracker.Name = "ListView_TimeTracker";
            this.ListView_TimeTracker.Size = new System.Drawing.Size(668, 375);
            this.ListView_TimeTracker.TabIndex = 2;
            this.ListView_TimeTracker.UseCompatibleStateImageBehavior = false;
            // 
            // Label_NowRunTimeValue
            // 
            this.Label_NowRunTimeValue.AutoSize = true;
            this.Label_NowRunTimeValue.Font = new System.Drawing.Font("Yu Gothic UI", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.Label_NowRunTimeValue.Location = new System.Drawing.Point(199, 41);
            this.Label_NowRunTimeValue.Name = "Label_NowRunTimeValue";
            this.Label_NowRunTimeValue.Size = new System.Drawing.Size(226, 21);
            this.Label_NowRunTimeValue.TabIndex = 4;
            this.Label_NowRunTimeValue.Text = "1990.01.01 00:00:00 (00:00:00)";
            // 
            // NotifyIcon
            // 
            this.NotifyIcon.Text = "TimeTracker";
            this.NotifyIcon.Visible = true;
            this.NotifyIcon.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.NotifyIcon_MouseDoubleClick);
            // 
            // Label_NowRunTime
            // 
            this.Label_NowRunTime.AutoSize = true;
            this.Label_NowRunTime.BackColor = System.Drawing.Color.Transparent;
            this.Label_NowRunTime.Font = new System.Drawing.Font("Yu Gothic UI", 8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(128)));
            this.Label_NowRunTime.Location = new System.Drawing.Point(9, 40);
            this.Label_NowRunTime.Name = "Label_NowRunTime";
            this.Label_NowRunTime.Size = new System.Drawing.Size(155, 21);
            this.Label_NowRunTime.TabIndex = 6;
            this.Label_NowRunTime.Text = "現在時刻 (実行時間):";
            // 
            // ToolStripMenuItem_DisplayCsvFolder
            // 
            this.ToolStripMenuItem_DisplayCsvFolder.Name = "ToolStripMenuItem_DisplayCsvFolder";
            this.ToolStripMenuItem_DisplayCsvFolder.Size = new System.Drawing.Size(232, 30);
            this.ToolStripMenuItem_DisplayCsvFolder.Text = "CSVフォルダを表示";
            this.ToolStripMenuItem_DisplayCsvFolder.Click += new System.EventHandler(this.ToolStripMenuItem_DisplayCsvFolder_Click);
            // 
            // ToolStripMenuItem_OutputCsv
            // 
            this.ToolStripMenuItem_OutputCsv.Name = "ToolStripMenuItem_OutputCsv";
            this.ToolStripMenuItem_OutputCsv.Size = new System.Drawing.Size(210, 30);
            this.ToolStripMenuItem_OutputCsv.Text = "CSV出力";
            this.ToolStripMenuItem_OutputCsv.Click += new System.EventHandler(this.ToolStripMenuItem_OutputCsv_Click);
            // 
            // TimeTracker
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(10F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.MenuBar;
            this.ClientSize = new System.Drawing.Size(678, 444);
            this.Controls.Add(this.Label_NowRunTime);
            this.Controls.Add(this.Label_NowRunTimeValue);
            this.Controls.Add(this.ListView_TimeTracker);
            this.Controls.Add(this.Menu_TimeTracker);
            this.Name = "TimeTracker";
            this.Text = "TimeTracker";
            this.TopMost = true;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.TimeTracker_FormClosing);
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.TimeTracker_FormClosed);
            this.Load += new System.EventHandler(this.TimeTracker_Load);
            this.Resize += new System.EventHandler(this.TimeTracker_Resize);
            this.Menu_TimeTracker.ResumeLayout(false);
            this.Menu_TimeTracker.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Timer TimerInterval;
        private System.Windows.Forms.MenuStrip Menu_TimeTracker;
        private System.Windows.Forms.ToolStripMenuItem ファイルFToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_TimeTracker_Exit;
        private System.Windows.Forms.ToolStripMenuItem 表示ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_TimeTracker_TopMost;
        private System.Windows.Forms.ListView ListView_TimeTracker;
        private System.Windows.Forms.Label Label_NowRunTimeValue;
        private System.Windows.Forms.NotifyIcon NotifyIcon;
        private System.Windows.Forms.Label Label_NowRunTime;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_DisplayCsvFolder;
        private System.Windows.Forms.ToolStripMenuItem ToolStripMenuItem_OutputCsv;
    }
}

