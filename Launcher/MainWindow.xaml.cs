using Ookii.Dialogs.Wpf;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Launcher
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void OpenClick(object sender, RoutedEventArgs e)
        {
            VistaFolderBrowserDialog dialog = new VistaFolderBrowserDialog();
            dialog.Description = "Select level folder";

            if ((bool)dialog.ShowDialog(this))
            {
                Process process = new Process();
                process.StartInfo.FileName = "Arrhythmia Studio.exe";
                process.StartInfo.Arguments = $"-open \"{dialog.SelectedPath}\"";
                process.Start();
                Application.Current.Shutdown();
            }
        }

        private void NewClick(object sender, RoutedEventArgs e)
        {
            NewLevelDialog nld = new NewLevelDialog();
            if ((bool)nld.ShowDialog())
            {
                Process process = new Process();
                process.StartInfo.FileName = "Arrhythmia Studio.exe";
                process.StartInfo.Arguments = $"-new \"{nld.LevelName}\" -level-path \"{nld.LevelPath}\" -song-path \"{nld.SongPath}\"";
                process.Start();
                Application.Current.Shutdown();
            }
        }
    }
}
