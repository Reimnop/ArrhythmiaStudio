namespace NotJSBEditor.GameLogic
{
    public class Scene
    {
        public SceneNode RootNode => _rootNode;

        private SceneNode _rootNode;

        public Scene()
        {
            _rootNode = new SceneNode("Scene Root");
        }
    }
}
