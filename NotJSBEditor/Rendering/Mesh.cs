using OpenTK.Graphics.OpenGL4;
using OpenTK.Mathematics;
using System;
using System.Runtime.CompilerServices;

namespace NotJSBEditor.Rendering
{
    public class Mesh : IDisposable
    {
        public Span<Vector3> Vertices => new Span<Vector3>(_vertices);
        public Span<uint> Indices => new Span<uint>(_indices);

        public int VerticesCount => _vertices.Length;
        public int IndicesCount => _indices.Length;

        public int VAO => _vao;
        public int VBO0 => _vbo0;
        public int VBO1 => _vbo1;
        public int EBO => _ebo;

        private Vector3[] _vertices;
        private uint[] _indices;

        private int _vao;
        private int _vbo0;
        private int _vbo1;
        private int _ebo;

        public Mesh(Vector3[] vertices, uint[] indices)
        {
            // Generate buffers
            _vao = GL.GenVertexArray();
            _vbo0 = GL.GenBuffer();
            _vbo1 = GL.GenBuffer();
            _ebo = GL.GenBuffer();

            // Upload buffers
            _vertices = vertices;
            _indices = indices;

            GL.BindVertexArray(_vao);

            // Upload vertex buffer
            GL.BindBuffer(BufferTarget.ArrayBuffer, _vbo0);
            GL.BufferData(BufferTarget.ArrayBuffer, Vector3.SizeInBytes * vertices.Length, vertices, BufferUsageHint.StaticDraw);

            // Upload element buffer
            GL.BindBuffer(BufferTarget.ElementArrayBuffer, _ebo);
            GL.BufferData(BufferTarget.ElementArrayBuffer, sizeof(uint) * indices.Length, indices, BufferUsageHint.StaticDraw);

            // Vertex attrib
            GL.VertexAttribPointer(0, 3, VertexAttribPointerType.Float, false, Vector3.SizeInBytes, 0);
            GL.EnableVertexAttribArray(0);

            GL.BindVertexArray(0);
            GL.BindBuffer(BufferTarget.ArrayBuffer, 0);
            GL.BindBuffer(BufferTarget.ElementArrayBuffer, 0);
        }

        public void SetPerVertexData<T>(T[] data, params VertexAttrib[] vertexAttribs) where T : struct
        {
            GL.BindVertexArray(_vao);

            GL.BindBuffer(BufferTarget.ArrayBuffer, _vbo1);
            GL.BufferData(BufferTarget.ArrayBuffer, Unsafe.SizeOf<T>() * data.Length, data, BufferUsageHint.StaticDraw);

            int offset = 0;
            for (int i = 0; i < vertexAttribs.Length; i++)
            {
                VertexAttrib attrib = vertexAttribs[i];

                GL.VertexAttribPointer(i + 1, attrib.Size, attrib.PointerType, attrib.Normalized, 0, offset);
                GL.EnableVertexAttribArray(i + 1);

                offset += attrib.SizeInBytes;
            }

            GL.BindVertexArray(0);
            GL.BindBuffer(BufferTarget.ArrayBuffer, 0);
        }

        public void Dispose()
        {
            GL.DeleteVertexArray(_vao);
            GL.DeleteBuffer(_vbo0);
            GL.DeleteBuffer(_vbo1);
            GL.DeleteBuffer(_ebo);
        }
    }
}