using OpenTK.Mathematics;
using OpenTK.Windowing.Common;
using OpenTK.Windowing.Desktop;
using System;

namespace NotJSBEditor
{
    internal class Program
    {
        private static void Main(string[] args)
        {
            GameWindowSettings gws = GameWindowSettings.Default;
            NativeWindowSettings nws = new NativeWindowSettings()
            {
                APIVersion = new Version(3, 3),
                Profile = ContextProfile.Core,
                Flags = ContextFlags.ForwardCompatible,
                Size = new Vector2i(1600, 900)
            };

            using (MainWindow window = new MainWindow(gws, nws))
            {
                window.Run();
            }
        }
    }
}
