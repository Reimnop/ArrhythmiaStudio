using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using Ookii.Dialogs.Wpf;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows.Media;
using System.IO;
using System.Windows;

namespace Launcher
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private List<LevelDetails> recentLevels = new List<LevelDetails>();

        public MainWindow()
        {
            InitializeComponent();
            LoadRecent();
        }

        private void LoadRecent()
        {
            if (File.Exists("recent.json"))
            {
                recentLevels.Clear();

                JArray arr = JArray.Parse(File.ReadAllText("recent.json"));

                foreach (JToken token in arr)
                {
                    recentLevels.Add(new LevelDetails(token));
                }
            }
            recentLevels.Sort((x, y) => y.LastOpened.CompareTo(x.LastOpened));
            UpdateRecentUI();
        }

        private void SaveRecent()
        {
            JArray arr = new JArray();
            foreach (LevelDetails levelDetails in recentLevels)
            {
                arr.Add(levelDetails.ToJson());
            }
            File.WriteAllText("recent.json", arr.ToString(Formatting.None));
        }

        private void UpdateRecentUI()
        {
            RecentPanel.Children.Clear();
            
            foreach (LevelDetails levelDetails in recentLevels)
            {
                LevelButton btn = new LevelButton();
                btn.Foreground = Brushes.White;
                btn.Height = 40.0;
                btn.LevelName = levelDetails.LevelName;
                btn.LastOpened = levelDetails.LastOpened.ToString("yyyy-MM-dd hh:mm tt");
                btn.LevelPath = levelDetails.LevelPath;
                btn.Click += LevelButton_Click;
                RecentPanel.Children.Add(btn);
            }
        }

        private void LevelButton_Click(object sender, RoutedEventArgs e)
        {
            LevelButton btn = e.Source as LevelButton;
            OpenLevel(btn.LevelPath);
            Application.Current.Shutdown();
        }

        private void OpenClick(object sender, RoutedEventArgs e)
        {
            VistaFolderBrowserDialog dialog = new VistaFolderBrowserDialog();
            dialog.Description = "Select level folder";

            if ((bool)dialog.ShowDialog(this))
            {
                OpenLevel(dialog.SelectedPath);
                Application.Current.Shutdown();
            }
        }

        private void NewClick(object sender, RoutedEventArgs e)
        {
            NewLevelDialog nld = new NewLevelDialog();
            if ((bool)nld.ShowDialog())
            {
                NewLevel(nld.LevelName, nld.LevelPath, nld.SongPath);
                Application.Current.Shutdown();
            }
        }

        private void OpenLevel(string path)
        {
            JObject levelJson = JObject.Parse(File.ReadAllText(Path.Combine(path, "level.aslv")));
            LevelDetails details = new LevelDetails()
            {
                LevelName = (string)levelJson["name"],
                LevelPath = path,
                LastOpened = DateTime.Now
            };
            recentLevels.Insert(0, details);
            SaveRecent();

            Process process = new Process();
            process.StartInfo.FileName = "Arrhythmia Studio.exe";
            process.StartInfo.Arguments = $"-open \"{path}\"";
            process.Start();
        }

        private void NewLevel(string levelName, string levelPath, string songPath)
        {
            LevelDetails details = new LevelDetails()
            {
                LevelName = (string)levelName,
                LevelPath = levelPath,
                LastOpened = DateTime.Now
            };
            recentLevels.Insert(0, details);
            SaveRecent();

            Process process = new Process();
            process.StartInfo.FileName = "Arrhythmia Studio.exe";
            process.StartInfo.Arguments = $"-new \"{levelName}\" -level-path \"{levelPath}\" -song-path \"{songPath}\"";
            process.Start();
        }
    }
}
