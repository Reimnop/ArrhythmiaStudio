using OpenTK.Mathematics;

namespace NotJSBEditor.Rendering
{
    public class Camera
    {
        public Vector3 Position = Vector3.Zero;
        public Quaternion Rotation = Quaternion.Identity;

        public float OrthographicScale = 10.0f;

        public float NearPlane = -64.0f;
        public float FarPlane = 64.0f;

        private Matrix4 model =>
            Matrix4.CreateFromQuaternion(Rotation) *
            Matrix4.CreateTranslation(Position);

        public void CalculateViewProjection(float aspect, out Matrix4 view, out Matrix4 projection)
        {
            view = Matrix4.Invert(model);
            projection = Matrix4.CreateOrthographic(OrthographicScale * aspect, OrthographicScale, NearPlane, FarPlane);
        }

        private Vector3 DegreesToRadians(Vector3 degrees)
        {
            return new Vector3(
                MathHelper.DegreesToRadians(degrees.X),
                MathHelper.DegreesToRadians(degrees.Y),
                MathHelper.DegreesToRadians(degrees.Z));
        }
    }
}
