using OpenTK.Mathematics;

namespace NotJSBEditor.GameLogic
{
    public class Transform
    {
        public Vector3 Position
        {
            get => _position;
            set
            {
                _position = value;
                RecalculateLocal();
            }
        }
        public Vector3 Scale
        {
            get => _scale;
            set
            {
                _scale = value;
                RecalculateLocal();
            }
        }
        public Quaternion Rotation
        {
            get => _rotation;
            set
            {
                //check equal
                if (_rotation == value)
                    return;

                _rotation = value;
                Quaternion.ToEulerAngles(value, out Vector3 euler);

                //convert to degrees
                _eulerAngles = new Vector3(
                    MathHelper.RadiansToDegrees(euler.X),
                    MathHelper.RadiansToDegrees(euler.Y),
                    MathHelper.RadiansToDegrees(euler.Z));

                RecalculateLocal();
            }
        }
        public Vector3 EulerAngles
        {
            get => _eulerAngles;
            set
            {
                if (_eulerAngles == value)
                    return;

                _eulerAngles = value;

                //convert to radians
                Vector3 radEuler = new Vector3(
                    MathHelper.DegreesToRadians(value.X),
                    MathHelper.DegreesToRadians(value.Y),
                    MathHelper.DegreesToRadians(value.Z));

                Quaternion.FromEulerAngles(radEuler, out _rotation);

                RecalculateLocal();
            }
        }
        public Matrix4 LocalModelMatrix => _localModelMatrix;

        private Vector3 _position = Vector3.Zero;
        private Vector3 _scale = Vector3.One;
        private Quaternion _rotation = Quaternion.Identity;
        private Vector3 _eulerAngles = Vector3.Zero;
        private Matrix4 _localModelMatrix = Matrix4.Identity;

        public SceneNode BaseNode => _baseNode;
        private SceneNode _baseNode;

        internal Transform(SceneNode baseNode)
        {
            _baseNode = baseNode;
        }

        private void RecalculateLocal()
        {
            _localModelMatrix = 
                Matrix4.CreateScale(_scale)
                * Matrix4.CreateFromQuaternion(_rotation)
                * Matrix4.CreateTranslation(_position);
        }
    }
}
