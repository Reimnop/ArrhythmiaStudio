using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;

namespace Launcher
{
    public class LevelButton : Button
    {
        public static DependencyProperty LevelNameProperty;
        public static DependencyProperty LastOpenedProperty;
        public static DependencyProperty LevelPathProperty;

        public string LevelName
        {
            get => (string)GetValue(LevelNameProperty);
            set => SetValue(LevelNameProperty, value);
        }

        public string LevelPath
        {
            get => (string)GetValue(LevelPathProperty);
            set => SetValue(LevelPathProperty, value);
        }

        public string LastOpened
        {
            get => (string)GetValue(LastOpenedProperty);
            set => SetValue(LastOpenedProperty, value);
        }

        static LevelButton()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(LevelButton), new FrameworkPropertyMetadata(typeof(LevelButton)));
            LevelNameProperty = DependencyProperty.Register("LevelName", typeof(string), typeof(LevelButton));
            LastOpenedProperty = DependencyProperty.Register("LastOpened", typeof(string), typeof(LevelButton));
            LevelPathProperty = DependencyProperty.Register("LevelPath", typeof(string), typeof(LevelButton));
        }
    }
}
