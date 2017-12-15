using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.IO;
using System.Drawing;

namespace TimeTracker
{
    public partial class TimeTracker : Form
    {
        private string BACKUP_DIR_PATH = ".\\backup";
        private int LISTVIEW_COLUMN_NUM = 2;
        private string[] LISTVIEW_COLUMN_TEXT = { "Active Time", "Process Name" };
        private int[] LISTVIEW_COLUMN_WIDTH = { 100, -1 };

        private DateTime NowTime = DateTime.Now;
        private DateTime RunTime = new DateTime();
        private List<ProcTimeLog> ProcTimeLogList = new List<ProcTimeLog>();

        //WIN32API:現在アクティブなウィンドウハンドルを取得
        [DllImport("user32.dll")]
        public static extern IntPtr GetForegroundWindow();

        //WIN32API:ウィンドウハンドルのプロセスIDを取得
        [DllImport("user32.dll")]
        public static extern int GetWindowThreadProcessId(IntPtr hWnd, out int lpdwProcessId);

        //コンストラクタ
        public TimeTracker()
        {
            InitializeComponent();
        }

        //WndProcオーバーライド
        protected override void WndProc(ref Message m)
        {
            if ((m.Msg == 0x112) && (m.WParam == (IntPtr)0xF020))
            {
                //最小化されたときにフォームを非表示にする
                this.Hide();
            }
            else
            {
                //その他はデフォルト処理
                base.WndProc(ref m);
            }
        }

        //ロードイベント
        private void TimeTracker_Load(object sender, EventArgs e)
        {
            //タスクトレイアイコンを設定
            NotifyIcon.Icon = SystemIcons.Application;

            //バックアップディレクトリ作成
            Directory.CreateDirectory(BACKUP_DIR_PATH);

            //リストビュー初期化
            InitListView();

            //バックアップから読み込み
            string filePath = GetBackupFilePath(NowTime);
            if (File.Exists(filePath))
            {
                csvRead(filePath);
            }

            //現在時刻と実行時間
            Label_NowRunTimeValue_Update();
        }

        //リサイズイベント
        private void TimeTracker_Resize(object sender, EventArgs e)
        {
            //リストビュー初期化(この中で列幅を調整)
            InitListView();
        }

        //フォーム終了前イベント
        private void TimeTracker_FormClosing(object sender, FormClosingEventArgs e)
        {
            //ダイアログを表示する
            DialogResult result = MessageBox.Show("ウィンドウを閉じますか？", "終了", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (result == DialogResult.No)
            {
                //「いいえ」押下されたため、フォーム終了をキャンセルする
                e.Cancel = true;
            }
        }

        //フォーム終了後イベント
        private void TimeTracker_FormClosed(object sender, FormClosedEventArgs e)
        {
            //CSVへバックアップする
            string filePath = GetBackupFilePath(NowTime);
            CsvWrite(filePath);

            //トレイリストのアイコンを非表示にする
            NotifyIcon.Visible = false;
        }

        //タスクトレイアイコンダブルクリックイベント
        private void NotifyIcon_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            //タスクトレイアイコンダブルクリックでフォームを表示
            this.Show();
            this.Activate();
        }

        //タイマーイベント
        private void TimerInterval_Tick(object sender, EventArgs e)
        {
            //日付が変わったかチェック
            if (!NowTime.Date.Equals(DateTime.Now.Date))
            {
                //バックアップ
                string filePath = GetBackupFilePath(NowTime);
                CsvWrite(filePath);

                //クリア
                ListView_TimeTracker.Items.Clear();
                ProcTimeLogList.Clear();
            }

            //時刻の差を取得
            TimeSpan ts = DateTime.Now - NowTime;

            //現在時刻と実行時間を更新
            NowTime = DateTime.Now;
            RunTime = RunTime.AddSeconds(ts.TotalSeconds);
            Label_NowRunTimeValue.Text = NowTime.ToString() + " (" + RunTime.ToLongTimeString() + ")";

            //現在アクティブなウィンドウのプロセスIDとプロセス名を取得
            int procId = GetActiveProcessId();
            string procName = GetActiveProcessName(procId);

            //現在アクティブなプロセス情報を作成
            ProcTimeLog ActiveProc = new ProcTimeLog(procName);

            //ProcTimeLogListに登録済みか検索
            int index = -1;
            for (int i = 0; i < ProcTimeLogList.Count; i++)
            {
                if (ProcTimeLogList[i].Equals(ActiveProc))
                {
                    index = i;
                    break;
                }
            }

            if (index < 0)
            {
                //未登録なので、現在アクティブなプロセス情報をリストに登録
                ProcTimeLogList.Add(ActiveProc);

                //リストビューに表示
                string[] row = { ActiveProc.DTime.ToLongTimeString(), ActiveProc.ProcName };
                ListView_TimeTracker.Items.Add(new ListViewItem(row));
            }
            else
            {
                //登録済みなので、現在アクティブなプロセスの実行時間を更新
                ProcTimeLogList[index].UpdateTime(ts.TotalSeconds);
                ListView_TimeTracker.Items[index].Text = ProcTimeLogList[index].DTime.ToLongTimeString();
            }
        }

        //「ファイル」→「CSV出力」釦押下イベント
        private void ToolStripMenuItem_OutputCsv_Click(object sender, EventArgs e)
        {
            string filePath = GetBackupFilePath(NowTime);
            CsvWrite(filePath);

            //ダイアログを表示する
            DialogResult result = MessageBox.Show("CSV出力完了" + Environment.NewLine + "出力したCSVファイルを開きますか？", "CSV出力完了", MessageBoxButtons.YesNo, MessageBoxIcon.Question);
            if (result == DialogResult.Yes)
            {
                //CSVファイルを開く
                Process.Start(filePath);
            }
        }

        //「ファイル」→「終了」釦押下イベント
        private void ToolStripMenuItem_TimeTracker_Exit_Click(object sender, EventArgs e)
        {
            //終了
            Close();
        }

        //「表示」→「CSVフォルダを表示」釦押下イベント
        private void ToolStripMenuItem_DisplayCsvFolder_Click(object sender, EventArgs e)
        {
            Process.Start(BACKUP_DIR_PATH);
        }

        //「表示」→「常に最前面に表示」釦押下イベント
        private void ToolStripMenuItem_TimeTracker_TopMost_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;

            //最前面状態を反転させる
            TopMost = !TopMost;

            //チェック状態を反転させる
            item.Checked = !item.Checked;
        }

        //Label_NowRunTimeValueを更新
        private void Label_NowRunTimeValue_Update()
        {
            Label_NowRunTimeValue.Text = NowTime.ToString() + " (" + RunTime.ToLongTimeString() + ")";
        }

        //リストビュー初期化
        private void InitListView()
        {
            if (ListView_TimeTracker.Columns.Count == 0)
            {
                //ListViewコントロールの設定
                ListView_TimeTracker.View = View.Details;

                //ListViewカラムの設定
                for(int i = 0; i < LISTVIEW_COLUMN_NUM; i++)
                {
                    ColumnHeader ch = new ColumnHeader();
                    ch.Text = LISTVIEW_COLUMN_TEXT[i];
                    ch.Width = LISTVIEW_COLUMN_WIDTH[i];
                    ListView_TimeTracker.Columns.Add(ch);
                }
            }

            int allColWidth = 0;
            foreach(int w in LISTVIEW_COLUMN_WIDTH)
            {
                if (w >= 0) allColWidth += w;
            }

            //最後の幅はリストビュー全体の幅から調整
            int last = LISTVIEW_COLUMN_NUM - 1;
            if (LISTVIEW_COLUMN_WIDTH[last] < 0)
            {
                ListView_TimeTracker.Columns[last].Width = ListView_TimeTracker.Width - allColWidth;
            }
        }

        //バックアップファイルパスを取得
        private string GetBackupFilePath(DateTime time)
        {
            return BACKUP_DIR_PATH + "\\" + time.Date.ToShortDateString().Replace("/", "") + ".csv";
        }

        //現在アクティブなウィンドウのプロセスIDを取得
        private int GetActiveProcessId()
        {
            int procId;
            try
            {
                //現在アクティブなウィンドウハンドルを取得
                IntPtr hWnd = GetForegroundWindow();

                //プロセスIDを取得
                GetWindowThreadProcessId(hWnd, out procId);
            }
            catch
            {
                procId = -1;
            }

            return procId;
        }

        //現在アクティブなウィンドウのプロセス名を取得
        private string GetActiveProcessName(int procId)
        {
            string procName;
            try
            {
                Process proc = Process.GetProcessById(procId);
                procName = proc.ProcessName;
            }
            catch
            {
                procName = "Unknown";
            }

            return procName;
        }

        //CSVから読み込み
        private void csvRead(string filePath)
        {
            if (File.Exists(filePath))
            {
                using (StreamReader sr = new StreamReader(filePath))
                {
                    //1行目は飛ばす
                    sr.ReadLine();

                    //ストリームの末尾まで繰り返す
                    while (!sr.EndOfStream)
                    {
                        //ファイルから一行読み込む
                        var line = sr.ReadLine();

                        // 読み込んだ一行をカンマ毎に分けて配列に格納する
                        var values = line.Split(',');
                        DateTime dTime = DateTime.Parse(values[0]);
                        string procName = values[1];

                        ProcTimeLog t = new ProcTimeLog(dTime, procName);
                        ProcTimeLogList.Add(t);

                        string[] row = { t.DTime.ToLongTimeString(), t.ProcName };
                        ListView_TimeTracker.Items.Add(new ListViewItem(row));

                        RunTime = RunTime.Add(dTime.TimeOfDay);
                    }
                    sr.Close();
                }
            }
        }

        //CSVへ書き込み
        private void CsvWrite(string filePath)
        {
            using (StreamWriter w = new StreamWriter(filePath))
            {
                w.WriteLine(string.Join(",", LISTVIEW_COLUMN_TEXT[0], LISTVIEW_COLUMN_TEXT[1]));
                foreach (ProcTimeLog t in ProcTimeLogList)
                {
                    w.WriteLine(string.Join(",", t.DTime.ToLongTimeString(), t.ProcName));
                }
                w.Close();
            }
        }
    }

    public class ProcTimeLog
    {
        public DateTime DTime { get; private set; }
        public string ProcName { get; }

        //コンストラクタ
        public ProcTimeLog()
        {
            DTime = new DateTime();
            ProcName = "";
        }
        public ProcTimeLog(string procName)
        {
            DTime = new DateTime();
            ProcName = procName;
        }
        public ProcTimeLog(DateTime dTime, string procName)
        {
            DTime = dTime;
            ProcName = procName;
        }

        //空判定
        public bool Empty()
        {
            bool empty = false;
            if (ProcName.Equals(""))
            {
                empty = true;
            }

            return empty;
        }

        //比較
        public bool Equals(ProcTimeLog proc)
        {
            bool equal = false;
            if (ProcName.Equals(proc.ProcName))
            {
                equal = true;
            }

            return equal;
        }

        //時間を更新
        public void UpdateTime(double seconds)
        {
            DTime = DTime.AddSeconds(seconds);
        }
    }
}
