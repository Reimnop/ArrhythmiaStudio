using NotJSBEditor.GameLogic;
using NotJSBEditor.Rendering;
using OpenTK.Graphics.OpenGL4;
using OpenTK.Mathematics;
using OpenTK.Windowing.Common;
using OpenTK.Windowing.Desktop;

namespace NotJSBEditor
{
    public class MainWindow : GameWindow
    {
        // Handles graphics
        private Renderer renderer;

        public MainWindow(GameWindowSettings gws, NativeWindowSettings nws) : base(gws, nws)
        {
        }

        protected override void OnLoad()
        {
            base.OnLoad();

            // Initialize modules
            Scene scene = new Scene();

            renderer = new Renderer(this, scene);

            // Init test data
            Vector3[] vertices = new Vector3[]
            {
                new Vector3(0.5f, 0.5f, 0.0f),
                new Vector3(-0.5f, 0.5f, 0.0f),
                new Vector3(-0.5f, -0.5f, 0.0f),
                new Vector3(0.5f, -0.5f, 0.0f)
            };

            uint[] indices = new uint[]
            {
                0, 1, 2,
                0, 2, 3
            };

            Mesh mesh = new Mesh(vertices, indices);
            Shader shader = new Shader("Assets/Shaders/basic.vert", "Assets/Shaders/basic.frag");
            Material material = new Material(shader);

            SceneNode node = new SceneNode("con cac", scene.RootNode);
            MeshRenderer meshRenderer = new MeshRenderer();
            meshRenderer.Mesh = mesh;
            meshRenderer.Material = material;

            node.Renderer = meshRenderer;
        }

        protected override void OnUpdateFrame(FrameEventArgs args)
        {
            base.OnUpdateFrame(args);

            // Updates everything
            float deltaTime = (float)args.Time;

            renderer.Update(deltaTime);
        }

        protected override void OnRenderFrame(FrameEventArgs args)
        {
            base.OnRenderFrame(args);

            GL.Viewport(0, 0, ClientSize.X, ClientSize.Y);
            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);

            renderer.RenderScene();

            SwapBuffers();
        }
    }
}
