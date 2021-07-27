using OpenTK.Mathematics;
using System.Runtime.InteropServices;

namespace NotJSBEditor.Rendering
{
    // Holds transformation matrices
    [StructLayout(LayoutKind.Explicit, Size = 256)]
    public struct TransformBuffer
    {
        [FieldOffset(0)]
        public Matrix4 Model;

        [FieldOffset(64)]
        public Matrix4 View;

        [FieldOffset(128)]
        public Matrix4 Projection;

        [FieldOffset(192)]
        public Matrix4 ModelViewProjection;
    }
}
