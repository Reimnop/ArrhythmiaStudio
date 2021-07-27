using OpenTK.Graphics.OpenGL4;

namespace NotJSBEditor.Rendering
{
    public struct VertexAttrib
    {
        public VertexAttribPointerType PointerType;
        public int Size;
        public int SizeInBytes;
        public bool Normalized;

        public VertexAttrib(VertexAttribPointerType pointerType, int size, int sizeInBytes, bool normalized)
        {
            PointerType = pointerType;
            Size = size;
            SizeInBytes = sizeInBytes;
            Normalized = normalized;
        }
    }
}
