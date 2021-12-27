using Ookii.Dialogs.Wpf;
using System;
using System.Collections.Generic;
using System.IO;
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
using System.Windows.Shapes;

namespace Launcher
{
    /// <summary>
    /// Interaction logic for NewLevelDialog.xaml
    /// </summary>
    public partial class NewLevelDialog : Window
    {
        public string LevelName => LevelNameBox.Text;
        public string LevelPath => LevelPathBox.Text;
        public string SongPath => SongPathBox.Text;

        public NewLevelDialog()
        {
            InitializeComponent();
        }

        private void LevelPathBrowse_Click(object sender, RoutedEventArgs e)
        {
            VistaFolderBrowserDialog fbd = new VistaFolderBrowserDialog();
            fbd.Description = "Select level folder";
            if ((bool)fbd.ShowDialog(this))
            {
                LevelPathBox.Text = fbd.SelectedPath;
            }
        }

        private void SongPathBrowse_Click(object sender, RoutedEventArgs e)
        {
            VistaOpenFileDialog ofd = new VistaOpenFileDialog();
            ofd.Filter = "Vorbis files (*.ogg)|*.ogg";
            if ((bool)ofd.ShowDialog(this))
            {
                SongPathBox.Text = ofd.FileName;
            }
        }

        private void CreateLevel_Click(object sender, RoutedEventArgs e)
        {
            if (string.IsNullOrEmpty(LevelName))
            {
                MessageBox.Show("Level name cannot be empty!");
                return;
            }

            if (string.IsNullOrEmpty(LevelPath))
            {
                MessageBox.Show("Level path cannot be empty!");
                return;
            }

            if (string.IsNullOrEmpty(SongPath))
            {
                MessageBox.Show("Level path cannot be empty!");
                return;
            }

            if (!File.Exists(SongPath))
            {
                MessageBox.Show("Could not find specified song!");
                return;
            }

            DialogResult = true;
            Close();
        }
    }
}
