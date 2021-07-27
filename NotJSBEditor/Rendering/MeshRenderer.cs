using OpenTK.Graphics.OpenGL4;
using System;

namespace NotJSBEditor.Rendering
{
    public class MeshRenderer : IRenderer
    {
        public Mesh Mesh;
        public Material Material;

        private int transformBuffer;

        private int[] uniformBuffers;

        // Initialize resources
        public MeshRenderer()
        {
            // Make a transform UBO
            transformBuffer = GL.GenBuffer();
            GL.BindBuffer(BufferTarget.UniformBuffer, transformBuffer);
            GL.BufferData(BufferTarget.UniformBuffer, 256, IntPtr.Zero, BufferUsageHint.DynamicCopy);
            GL.BindBuffer(BufferTarget.UniformBuffer, 0);

            // Initialize buffer array
            uniformBuffers = new int[]
            {
                transformBuffer
            };
        }

        public bool Render(InputDrawData drawData, out OutputDrawData outDrawData)
        {
            outDrawData = new OutputDrawData();

            if (!CanRender())
                return false;

            // Construct transform buffer struct
            TransformBuffer transform = new TransformBuffer()
            {
                Model = drawData.Model,
                View = drawData.View,
                Projection = drawData.Projection,
                ModelViewProjection = drawData.ModelViewProjection
            };

            // Update UBO
            UpdateTransformBufer(transform);

            // Construct the draw data for rendering
            outDrawData = new OutputDrawData()
            {
                VAO = Mesh.VAO,
                Shader = Material.Shader.Handle,
                UniformBuffers = uniformBuffers,
                Command = new DrawElementsCommand()
                {
                    Count = Mesh.IndicesCount,
                    Offset = 0
                }
            };

            return true;
        }

        private void UpdateTransformBufer(TransformBuffer transform)
        {
            GL.BindBuffer(BufferTarget.UniformBuffer, transformBuffer);
            GL.BufferSubData(BufferTarget.UniformBuffer, IntPtr.Zero, 256, ref transform);
            GL.BindBuffer(BufferTarget.UniformBuffer, 0);
        }

        // Check if rendering is possible
        private bool CanRender()
        {
            if (Mesh == null || Material == null)
                return false;

            return true;
        }

        // Free unmanaged resources
        public void Dispose()
        {
            GL.DeleteBuffer(transformBuffer);
        }
    }
}