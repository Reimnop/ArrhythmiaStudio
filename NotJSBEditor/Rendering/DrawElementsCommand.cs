namespace NotJSBEditor.Rendering
{
    // GL.DrawElements
    public struct DrawElementsCommand : IDrawCommand
    {
        public int Count;
        public int Offset;
    }
}
