namespace NotJSBEditor.Rendering
{
    // Data necessary for drawing
    public struct OutputDrawData
    {
        public int VAO;
        public int Shader;
        public int[] UniformBuffers;
        public IDrawCommand Command;
    }
}
