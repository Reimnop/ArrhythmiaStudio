using OpenTK.Graphics.OpenGL4;
using System;
using System.Collections.Generic;
using System.IO;

namespace NotJSBEditor.Rendering
{
    public class Shader : IDisposable
    {
        public int Handle => _handle;

        private int _handle;

        private readonly Dictionary<string, int> uniformLocations;

        public Shader(string vertPath, string fragPath)
        {
            var vertexShader = GL.CreateShader(ShaderType.VertexShader);
            GL.ShaderSource(vertexShader, File.ReadAllText(vertPath));
            GL.CompileShader(vertexShader);

            var fragmentShader = GL.CreateShader(ShaderType.FragmentShader);
            GL.ShaderSource(fragmentShader, File.ReadAllText(fragPath));
            GL.CompileShader(fragmentShader);

            _handle = GL.CreateProgram();

            GL.AttachShader(_handle, vertexShader);
            GL.AttachShader(_handle, fragmentShader);
            GL.LinkProgram(_handle);

            GL.DetachShader(_handle, vertexShader);
            GL.DetachShader(_handle, fragmentShader);
            GL.DeleteShader(fragmentShader);
            GL.DeleteShader(vertexShader);

            GL.GetProgram(_handle, GetProgramParameterName.ActiveUniforms, out var numberOfUniforms);

            uniformLocations = new Dictionary<string, int>();

            for (var i = 0; i < numberOfUniforms; i++)
            {
                var key = GL.GetActiveUniform(_handle, i, out _, out _);
                var location = GL.GetUniformLocation(_handle, key);

                uniformLocations.Add(key, location);
            }
        }

        public int GetAttribLocation(string attribName)
        {
            if (uniformLocations.TryGetValue(attribName, out int loc))
                return loc;

            int attribLoc = GL.GetAttribLocation(_handle, attribName);
            if (attribLoc != -1)
                uniformLocations.Add(attribName, attribLoc);

            return attribLoc;
        }

        public void Dispose()
        {
            GL.DeleteProgram(_handle);
        }
    }
}
