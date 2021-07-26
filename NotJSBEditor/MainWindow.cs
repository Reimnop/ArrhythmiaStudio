using ImGuiOpenTK;
using OpenTK.Graphics.OpenGL4;
using OpenTK.Windowing.Common;
using OpenTK.Windowing.Desktop;

namespace NotJSBEditor
{
    public class MainWindow : GameWindow
    {
        private ImGuiController ImGuiController;

        public MainWindow(GameWindowSettings gws, NativeWindowSettings nws) : base(gws, nws)
        {
        }

        protected override void OnLoad()
        {
            base.OnLoad();

            ImGuiController = new ImGuiController(this, "Assets/Inconsolata.ttf");

            ImGuiController.OnLayout += ImGuiController_OnLayout;
        }

        private void ImGuiController_OnLayout()
        {
            ImGuiNET.ImGui.ShowDemoWindow();
        }

        protected override void OnUpdateFrame(FrameEventArgs args)
        {
            base.OnUpdateFrame(args);

            ImGuiController.Update((float)args.Time);
        }

        protected override void OnRenderFrame(FrameEventArgs args)
        {
            base.OnRenderFrame(args);

            GL.Viewport(0, 0, ClientSize.X, ClientSize.Y);
            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);
            
            ImGuiController.RenderImGui();

            SwapBuffers();
        }
    }
}
