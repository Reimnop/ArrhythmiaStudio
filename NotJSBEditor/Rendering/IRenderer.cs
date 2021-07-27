using System;

namespace NotJSBEditor.Rendering
{
    public interface IRenderer : IDisposable
    {
        // Returns a bool so we can discard
        public bool Render(InputDrawData drawData, out OutputDrawData outDrawData);
    }
}
