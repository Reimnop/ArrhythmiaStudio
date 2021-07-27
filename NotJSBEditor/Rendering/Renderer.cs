using ImGuiOpenTK;
using NotJSBEditor.GameLogic;
using OpenTK.Graphics.OpenGL4;
using OpenTK.Mathematics;
using OpenTK.Windowing.Desktop;
using System.Collections.Generic;

namespace NotJSBEditor.Rendering
{
    public class Renderer
    {
        // Contains queued draw data
        private List<OutputDrawData> queuedDrawData = new List<OutputDrawData>();

        private GameWindow mainWindow;
        private Scene mainScene;
        private ImGuiController imGuiController;

        private Camera camera;
        
        public Renderer(GameWindow window, Scene scene)
        {
            mainWindow = window;
            mainScene = scene;

            // Initialize rendering
            camera = new Camera();
            imGuiController = new ImGuiController(window, "Assets/Inconsolata.ttf");
        }

        // Runs every frame
        public void Update(float deltaTime)
        {
            imGuiController.Update(deltaTime);
        }

        // Renders entire scene
        public void RenderScene()
        {
            float aspect = mainWindow.ClientSize.X / (float)mainWindow.ClientSize.Y;
            camera.CalculateViewProjection(aspect, out Matrix4 view, out Matrix4 projection);
            RecursivelyRenderNodes(mainScene.RootNode, Matrix4.Identity, view, projection);

            // Loop and render all
            foreach (OutputDrawData drawData in queuedDrawData)
            {
                DrawElementsCommand drawCommand = (DrawElementsCommand)drawData.Command;

                GL.BindVertexArray(drawData.VAO);
                GL.UseProgram(drawData.Shader);

                // Bind all uniform buffers
                if (drawData.UniformBuffers != null)
                {
                    for (int i = 0; i < drawData.UniformBuffers.Length; i++)
                    {
                        GL.BindBufferBase(BufferRangeTarget.UniformBuffer, i, drawData.UniformBuffers[i]);
                    }
                }

                GL.DrawElements(PrimitiveType.Triangles, drawCommand.Count, DrawElementsType.UnsignedInt, drawCommand.Offset);
            }

            // Clean up
            queuedDrawData.Clear();

            // Render ImGui last
            imGuiController.RenderImGui();
        }

        // Recursively gather data from scene nodes for rendering
        private void RecursivelyRenderNodes(SceneNode node, Matrix4 parentTransform, Matrix4 view, Matrix4 projection)
        {
            Matrix4 nodeTransform = node.Transform.LocalModelMatrix;
            Matrix4 globalTransform = nodeTransform * parentTransform;

            // If the node isn't drawable, skip
            if (node.Renderer != null)
            {
                // Construct draw data struct
                InputDrawData drawData = new InputDrawData()
                {
                    Model = globalTransform,
                    View = view,
                    Projection = projection,
                    ModelViewProjection = globalTransform * view * projection
                };

                // Queue draw data if there is one
                if (node.Renderer.Render(drawData, out OutputDrawData outDrawData))
                {
                    queuedDrawData.Add(outDrawData);
                }
            }

            // Do the same for children, recursively
            foreach (SceneNode child in node.Children)
            {
                RecursivelyRenderNodes(child, globalTransform, view, projection);
            }
        }
    }
}
