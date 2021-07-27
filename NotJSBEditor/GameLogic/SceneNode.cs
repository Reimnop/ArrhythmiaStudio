using NotJSBEditor.Rendering;
using System.Collections.Generic;

namespace NotJSBEditor.GameLogic
{
    // Represents a node in a scene graph
    public class SceneNode
    {
        public string Name = "Untitled Node";

        // The node's transform
        public Transform Transform { get; }

        public SceneNode Parent
        {
            get => _parent;
            set
            {
                // If new parent is the same as old parent, do nothing
                if (value == _parent)
                    return;

                // Remove from old parent
                _parent?.Children.Remove(this);

                // Add to new parent
                _parent = value;
                _parent?.Children.Add(this);
            }
        }
        private SceneNode _parent;

        public List<SceneNode> Children { get; }

        // The node's renderer
        public IRenderer Renderer;

        public SceneNode(string name, SceneNode parent = null)
        {
            Name = name;
            Parent = parent;

            Transform = new Transform(this);
            Children = new List<SceneNode>();
        }

        // Destroy this node and its children
        public void Destroy()
        {
            // Unparent node
            Parent = null;

            // Clean renderer resources
            Renderer?.Dispose();

            // Destroy children
            foreach (SceneNode child in Children)
            {
                child.DestroyChild();
            }
        }

        private void DestroyChild()
        {
            // Clean renderer resources
            Renderer?.Dispose();

            // Destroy children
            foreach (SceneNode child in Children)
            {
                child.DestroyChild();
            }
        }
    }
}
