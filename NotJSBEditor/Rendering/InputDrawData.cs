using OpenTK.Mathematics;

namespace NotJSBEditor.Rendering
{
    // Contains all neccessary data for rendering
    public struct InputDrawData
    {
        public Matrix4 Model;
        public Matrix4 View;
        public Matrix4 Projection;
        public Matrix4 ModelViewProjection;
    }
}