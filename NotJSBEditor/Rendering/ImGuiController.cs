using ImGuiNET;
using OpenTK.Graphics.OpenGL4;
using OpenTK.Mathematics;
using OpenTK.Windowing.Common;
using OpenTK.Windowing.Common.Input;
using OpenTK.Windowing.Desktop;
using OpenTK.Windowing.GraphicsLibraryFramework;
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using Vector2 = System.Numerics.Vector2;
using Vector4 = System.Numerics.Vector4;

namespace ImGuiOpenTK
{
    public class ImGuiController
    {
        public static event Action OnLayout;

        private readonly string vertShader = 
@"#version 330

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aColor;

uniform mat4 projection;

out vec4 Color;
out vec2 TexCoord;

void main()
{
    Color = aColor;
    TexCoord = aTexCoord;
    gl_Position = vec4(aPosition, 0.0, 1.0) * projection;
}";

        private readonly string fragShader = 
@"#version 330

layout(location = 0) out vec4 fragColor;

uniform sampler2D inTexture;

in vec4 Color;
in vec2 TexCoord;

void main()
{
    fragColor = texture(inTexture, TexCoord) * Color;
}";

        private GameWindow mainWindow;
        
        private int windowWidth => mainWindow.ClientSize.X;
        private int windowHeight => mainWindow.ClientSize.Y;

        private int meshVAO;
        private int meshVBO;
        private int meshEBO;

        private int lastVBSize;
        private int lastIBSize;

        private int fontTexture;

        private int shader;
        private int projectionLoc;
        private int textureLoc;

        private Keys[] keys;
        private List<char> pressedChars = new List<char>();

        private bool frameBegun;

        public ImGuiController(GameWindow window, string fontPath)
        {
            mainWindow = window;

            // Create and set ImGui context
            IntPtr context = ImGui.CreateContext();
            ImGui.SetCurrentContext(context);
            
            ImGuiIOPtr io = ImGui.GetIO();

            // Set font
            io.Fonts.AddFontFromFileTTF(fontPath, 16);

            // Set ImGui configs
            io.BackendFlags |= ImGuiBackendFlags.HasMouseCursors | ImGuiBackendFlags.RendererHasVtxOffset;
            io.ConfigFlags |= ImGuiConfigFlags.DockingEnable | ImGuiConfigFlags.NavEnableKeyboard;
            io.ConfigWindowsResizeFromEdges = true;

            // Get all OpenTK keys
            keys = (Keys[])Enum.GetValues(typeof(Keys));

            // Subscribe to text input event
            mainWindow.TextInput += TextInput;

            SetStyle();

            CreateDeviceResources();
            SetKeyMappings();

            SetPerFrameImGuiData(0.166666f);

            ImGui.NewFrame();
            frameBegun = true;
        }

        private void SetStyle()
        {
            ImGuiStylePtr style = ImGui.GetStyle();
            RangeAccessor<Vector4> colors = style.Colors;

            colors[(int)ImGuiCol.Text] = new Vector4(1.000f, 1.000f, 1.000f, 1.000f);
            colors[(int)ImGuiCol.TextDisabled] = new Vector4(0.500f, 0.500f, 0.500f, 1.000f);
            colors[(int)ImGuiCol.WindowBg] = new Vector4(0.180f, 0.180f, 0.180f, 1.000f);
            colors[(int)ImGuiCol.ChildBg] = new Vector4(0.280f, 0.280f, 0.280f, 0.000f);
            colors[(int)ImGuiCol.PopupBg] = new Vector4(0.313f, 0.313f, 0.313f, 1.000f);
            colors[(int)ImGuiCol.Border] = new Vector4(0.266f, 0.266f, 0.266f, 1.000f);
            colors[(int)ImGuiCol.BorderShadow] = new Vector4(0.000f, 0.000f, 0.000f, 0.000f);
            colors[(int)ImGuiCol.FrameBg] = new Vector4(0.160f, 0.160f, 0.160f, 1.000f);
            colors[(int)ImGuiCol.FrameBgHovered] = new Vector4(0.200f, 0.200f, 0.200f, 1.000f);
            colors[(int)ImGuiCol.FrameBgActive] = new Vector4(0.280f, 0.280f, 0.280f, 1.000f);
            colors[(int)ImGuiCol.TitleBg] = new Vector4(0.148f, 0.148f, 0.148f, 1.000f);
            colors[(int)ImGuiCol.TitleBgActive] = new Vector4(0.148f, 0.148f, 0.148f, 1.000f);
            colors[(int)ImGuiCol.TitleBgCollapsed] = new Vector4(0.148f, 0.148f, 0.148f, 1.000f);
            colors[(int)ImGuiCol.MenuBarBg] = new Vector4(0.195f, 0.195f, 0.195f, 1.000f);
            colors[(int)ImGuiCol.ScrollbarBg] = new Vector4(0.160f, 0.160f, 0.160f, 1.000f);
            colors[(int)ImGuiCol.ScrollbarGrab] = new Vector4(0.277f, 0.277f, 0.277f, 1.000f);
            colors[(int)ImGuiCol.ScrollbarGrabHovered] = new Vector4(0.300f, 0.300f, 0.300f, 1.000f);
            colors[(int)ImGuiCol.ScrollbarGrabActive] = new Vector4(1.000f, 0.391f, 0.000f, 1.000f);
            colors[(int)ImGuiCol.CheckMark] = new Vector4(1.000f, 1.000f, 1.000f, 1.000f);
            colors[(int)ImGuiCol.SliderGrab] = new Vector4(0.391f, 0.391f, 0.391f, 1.000f);
            colors[(int)ImGuiCol.SliderGrabActive] = new Vector4(1.000f, 0.391f, 0.000f, 1.000f);
            colors[(int)ImGuiCol.Button] = new Vector4(1.000f, 1.000f, 1.000f, 0.000f);
            colors[(int)ImGuiCol.ButtonHovered] = new Vector4(1.000f, 1.000f, 1.000f, 0.156f);
            colors[(int)ImGuiCol.ButtonActive] = new Vector4(1.000f, 1.000f, 1.000f, 0.391f);
            colors[(int)ImGuiCol.Header] = new Vector4(0.313f, 0.313f, 0.313f, 1.000f);
            colors[(int)ImGuiCol.HeaderHovered] = new Vector4(0.469f, 0.469f, 0.469f, 1.000f);
            colors[(int)ImGuiCol.HeaderActive] = new Vector4(0.469f, 0.469f, 0.469f, 1.000f);
            colors[(int)ImGuiCol.Separator] = colors[(int)ImGuiCol.Border];
            colors[(int)ImGuiCol.SeparatorHovered] = new Vector4(0.391f, 0.391f, 0.391f, 1.000f);
            colors[(int)ImGuiCol.SeparatorActive] = new Vector4(1.000f, 0.391f, 0.000f, 1.000f);
            colors[(int)ImGuiCol.ResizeGrip] = new Vector4(1.000f, 1.000f, 1.000f, 0.250f);
            colors[(int)ImGuiCol.ResizeGripHovered] = new Vector4(1.000f, 1.000f, 1.000f, 0.670f);
            colors[(int)ImGuiCol.ResizeGripActive] = new Vector4(1.000f, 0.391f, 0.000f, 1.000f);
            colors[(int)ImGuiCol.Tab] = new Vector4(0.098f, 0.098f, 0.098f, 1.000f);
            colors[(int)ImGuiCol.TabHovered] = new Vector4(0.352f, 0.352f, 0.352f, 1.000f);
            colors[(int)ImGuiCol.TabActive] = new Vector4(0.195f, 0.195f, 0.195f, 1.000f);
            colors[(int)ImGuiCol.TabUnfocused] = new Vector4(0.098f, 0.098f, 0.098f, 1.000f);
            colors[(int)ImGuiCol.TabUnfocusedActive] = new Vector4(0.195f, 0.195f, 0.195f, 1.000f);
            colors[(int)ImGuiCol.DockingPreview] = new Vector4(1.000f, 0.391f, 0.000f, 0.781f);
            colors[(int)ImGuiCol.DockingEmptyBg] = new Vector4(0.180f, 0.180f, 0.180f, 1.000f);
            colors[(int)ImGuiCol.PlotLines] = new Vector4(0.469f, 0.469f, 0.469f, 1.000f);
            colors[(int)ImGuiCol.PlotLinesHovered] = new Vector4(1.000f, 0.391f, 0.000f, 1.000f);
            colors[(int)ImGuiCol.PlotHistogram] = new Vector4(0.586f, 0.586f, 0.586f, 1.000f);
            colors[(int)ImGuiCol.PlotHistogramHovered] = new Vector4(1.000f, 0.391f, 0.000f, 1.000f);
            colors[(int)ImGuiCol.TextSelectedBg] = new Vector4(1.000f, 1.000f, 1.000f, 0.156f);
            colors[(int)ImGuiCol.DragDropTarget] = new Vector4(1.000f, 0.391f, 0.000f, 1.000f);
            colors[(int)ImGuiCol.NavHighlight] = new Vector4(1.000f, 0.391f, 0.000f, 1.000f);
            colors[(int)ImGuiCol.NavWindowingHighlight] = new Vector4(1.000f, 0.391f, 0.000f, 1.000f);
            colors[(int)ImGuiCol.NavWindowingDimBg] = new Vector4(0.000f, 0.000f, 0.000f, 0.586f);
            colors[(int)ImGuiCol.ModalWindowDimBg] = new Vector4(0.000f, 0.000f, 0.000f, 0.586f);

            style.ChildRounding = 4.0f;
            style.FrameBorderSize = 1.0f;
            style.FrameRounding = 2.0f;
            style.GrabMinSize = 7.0f;
            style.PopupRounding = 2.0f;
            style.ScrollbarRounding = 12.0f;
            style.ScrollbarSize = 13.0f;
            style.TabBorderSize = 1.0f;
            style.TabRounding = 0.0f;
            style.WindowRounding = 4.0f;
        }

        private void TextInput(TextInputEventArgs obj)
        {
            pressedChars.Add((char)obj.Unicode);
        }

        private void CreateDeviceResources()
        {
            // Generate imgui mesh
            meshVAO = GL.GenVertexArray();
            GL.BindVertexArray(meshVAO);

            // Generate vbo
            meshVBO = GL.GenBuffer();
            GL.BindBuffer(BufferTarget.ArrayBuffer, meshVBO);
            GL.BufferData(BufferTarget.ArrayBuffer, 0, IntPtr.Zero, BufferUsageHint.DynamicDraw);

            // Generate ebo
            meshEBO = GL.GenBuffer();
            GL.BindBuffer(BufferTarget.ElementArrayBuffer, meshEBO);
            GL.BufferData(BufferTarget.ElementArrayBuffer, 0, IntPtr.Zero, BufferUsageHint.DynamicDraw);

            // Set up vertex attribs
            int stride = sizeof(float) * 4 + sizeof(uint);

            GL.VertexAttribPointer(0, 2, VertexAttribPointerType.Float, false, stride, 0); //aPosition
            GL.EnableVertexAttribArray(0);

            GL.VertexAttribPointer(1, 2, VertexAttribPointerType.Float, false, stride, sizeof(float) * 2); //aTexCoord
            GL.EnableVertexAttribArray(1);

            GL.VertexAttribPointer(2, 4, VertexAttribPointerType.UnsignedByte, true, stride, sizeof(float) * 4); //aColor
            GL.EnableVertexAttribArray(2);

            GL.BindVertexArray(0);
            GL.BindBuffer(BufferTarget.ArrayBuffer, 0);
            GL.BindBuffer(BufferTarget.ElementArrayBuffer, 0);

            // Compile imgui shaders
            int vertexShader = GL.CreateShader(ShaderType.VertexShader);
            GL.ShaderSource(vertexShader, vertShader);
            GL.CompileShader(vertexShader);

            int fragmentShader = GL.CreateShader(ShaderType.FragmentShader);
            GL.ShaderSource(fragmentShader, fragShader);
            GL.CompileShader(fragmentShader);

            shader = GL.CreateProgram();
            GL.AttachShader(shader, vertexShader);
            GL.AttachShader(shader, fragmentShader);
            GL.LinkProgram(shader);

            // Cleanup shader
            GL.DetachShader(shader, vertexShader);
            GL.DetachShader(shader, fragmentShader);
            GL.DeleteShader(vertexShader);
            GL.DeleteShader(fragmentShader);

            // Get uniform locations
            projectionLoc = GL.GetUniformLocation(shader, "projection");
            textureLoc = GL.GetUniformLocation(shader, "inTexture");

            RecreateFontDeviceTexture();
        }

        private void RecreateFontDeviceTexture()
        {
            ImGuiIOPtr io = ImGui.GetIO();
            io.Fonts.GetTexDataAsRGBA32(out IntPtr pixels, out int width, out int height, out _);

            fontTexture = GL.GenTexture();
            GL.BindTexture(TextureTarget.Texture2D, fontTexture);
            GL.TexImage2D(TextureTarget.Texture2D, 0, PixelInternalFormat.Rgba, width, height, 0, PixelFormat.Rgba, PixelType.UnsignedByte, pixels);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMinFilter, (int)TextureMinFilter.Linear);
            GL.TexParameter(TextureTarget.Texture2D, TextureParameterName.TextureMagFilter, (int)TextureMagFilter.Linear);
            GL.BindTexture(TextureTarget.Texture2D, 0);

            io.Fonts.SetTexID((IntPtr)fontTexture);

            io.Fonts.ClearTexData();
        }

        internal void RenderImGui()
        {
            OnLayout?.Invoke();       

            if (frameBegun)
            {
                frameBegun = false;
                ImGui.Render();
                RenderImDrawData(ImGui.GetDrawData());
            }
        }

        internal void Update(float dt)
        {
            if (frameBegun)
            {
                ImGui.Render();
            }

            SetPerFrameImGuiData(dt);
            UpdateImGuiInput();

            frameBegun = true;
            ImGui.NewFrame();
        }

        private void SetPerFrameImGuiData(float dt)
        {
            ImGuiIOPtr io = ImGui.GetIO();
            io.DisplaySize = new Vector2(windowWidth, windowHeight);
            io.DisplayFramebufferScale = Vector2.One;
            io.DeltaTime = dt;
        }

        private void UpdateImGuiInput()
        {
            ImGuiIOPtr io = ImGui.GetIO();

            MouseState mouseState = mainWindow.MouseState;
            KeyboardState keyboardState = mainWindow.KeyboardState;

            ImGuiMouseCursor cursor = ImGui.GetMouseCursor();

            switch (cursor)
            {
                case ImGuiMouseCursor.Arrow:
                    mainWindow.Cursor = MouseCursor.Default;
                    break;
                case ImGuiMouseCursor.Hand:
                    mainWindow.Cursor = MouseCursor.Hand;
                    break;
                case ImGuiMouseCursor.TextInput:
                    mainWindow.Cursor = MouseCursor.IBeam;
                    break;
                case ImGuiMouseCursor.ResizeNS:
                    mainWindow.Cursor = MouseCursor.VResize;
                    break;
                case ImGuiMouseCursor.ResizeEW:
                    mainWindow.Cursor = MouseCursor.HResize;
                    break;
                default:
                    mainWindow.Cursor = MouseCursor.Default;
                    break;
            }

            io.MouseDown[0] = mouseState.IsButtonDown(MouseButton.Left);
            io.MouseDown[1] = mouseState.IsButtonDown(MouseButton.Right);
            io.MouseDown[2] = mouseState.IsButtonDown(MouseButton.Middle);

            var point = mouseState.Position;
            io.MousePos = new Vector2(point.X, point.Y);

            io.MouseWheel = mouseState.ScrollDelta.Y;
            io.MouseWheelH = mouseState.ScrollDelta.X;

            foreach (Keys key in keys)
            {
                if (key != Keys.Unknown)
                {
                    io.KeysDown[(int)key] = keyboardState.IsKeyDown(key);
                }
            }

            foreach (char c in pressedChars)
            {
                io.AddInputCharacter(c);
            }
            pressedChars.Clear();

            io.KeyCtrl = keyboardState.IsKeyDown(Keys.LeftControl) || keyboardState.IsKeyDown(Keys.RightControl);
            io.KeyAlt = keyboardState.IsKeyDown(Keys.LeftAlt) || keyboardState.IsKeyDown(Keys.RightAlt);
            io.KeyShift = keyboardState.IsKeyDown(Keys.LeftShift) || keyboardState.IsKeyDown(Keys.RightShift);
            io.KeySuper = keyboardState.IsKeyDown(Keys.LeftSuper) || keyboardState.IsKeyDown(Keys.RightSuper);
        }

        private static void SetKeyMappings()
        {
            ImGuiIOPtr io = ImGui.GetIO();
            io.KeyMap[(int)ImGuiKey.Tab] = (int)Keys.Tab;
            io.KeyMap[(int)ImGuiKey.LeftArrow] = (int)Keys.Left;
            io.KeyMap[(int)ImGuiKey.RightArrow] = (int)Keys.Right;
            io.KeyMap[(int)ImGuiKey.UpArrow] = (int)Keys.Up;
            io.KeyMap[(int)ImGuiKey.DownArrow] = (int)Keys.Down;
            io.KeyMap[(int)ImGuiKey.PageUp] = (int)Keys.PageUp;
            io.KeyMap[(int)ImGuiKey.PageDown] = (int)Keys.PageDown;
            io.KeyMap[(int)ImGuiKey.Home] = (int)Keys.Home;
            io.KeyMap[(int)ImGuiKey.End] = (int)Keys.End;
            io.KeyMap[(int)ImGuiKey.Delete] = (int)Keys.Delete;
            io.KeyMap[(int)ImGuiKey.Backspace] = (int)Keys.Backspace;
            io.KeyMap[(int)ImGuiKey.Enter] = (int)Keys.Enter;
            io.KeyMap[(int)ImGuiKey.Space] = (int)Keys.Space;
            io.KeyMap[(int)ImGuiKey.Escape] = (int)Keys.Escape;
            io.KeyMap[(int)ImGuiKey.A] = (int)Keys.A;
            io.KeyMap[(int)ImGuiKey.C] = (int)Keys.C;
            io.KeyMap[(int)ImGuiKey.V] = (int)Keys.V;
            io.KeyMap[(int)ImGuiKey.X] = (int)Keys.X;
            io.KeyMap[(int)ImGuiKey.Y] = (int)Keys.Y;
            io.KeyMap[(int)ImGuiKey.Z] = (int)Keys.Z;
        }

        private void RenderImDrawData(ImDrawDataPtr drawData)
        {
            if (drawData.CmdListsCount == 0)
                return;

            // Calculate buffer sizes
            int totalVBSize = drawData.TotalVtxCount * Unsafe.SizeOf<ImDrawVert>();
            int totalIBSize = drawData.TotalIdxCount * sizeof(ushort);

            // Resize buffers
            if (totalVBSize > lastVBSize)
            {
                GL.BindBuffer(BufferTarget.ArrayBuffer, meshVBO);
                GL.BufferData(BufferTarget.ArrayBuffer, totalVBSize, IntPtr.Zero, BufferUsageHint.DynamicDraw);
                GL.BindBuffer(BufferTarget.ArrayBuffer, 0);

                lastVBSize = totalVBSize;
            }
            
            if (totalIBSize > lastIBSize)
            {
                GL.BindBuffer(BufferTarget.ElementArrayBuffer, meshEBO);
                GL.BufferData(BufferTarget.ElementArrayBuffer, totalIBSize, IntPtr.Zero, BufferUsageHint.DynamicDraw);
                GL.BindBuffer(BufferTarget.ElementArrayBuffer, 0);

                lastIBSize = totalIBSize;
            }

            // Update buffers with new data
            int vertexOffsetInVertices = 0;
            int indexOffsetInElements = 0;

            GL.BindBuffer(BufferTarget.ArrayBuffer, meshVBO);
            GL.BindBuffer(BufferTarget.ElementArrayBuffer, meshEBO);
            for (int i = 0; i < drawData.CmdListsCount; i++)
            {
                ImDrawListPtr cmdList = drawData.CmdListsRange[i];

                GL.BufferSubData(BufferTarget.ArrayBuffer, (IntPtr)(vertexOffsetInVertices * Unsafe.SizeOf<ImDrawVert>()), cmdList.VtxBuffer.Size * Unsafe.SizeOf<ImDrawVert>(), cmdList.VtxBuffer.Data);
                GL.BufferSubData(BufferTarget.ElementArrayBuffer, (IntPtr)(indexOffsetInElements * sizeof(ushort)), cmdList.IdxBuffer.Size * sizeof(ushort), cmdList.IdxBuffer.Data);

                vertexOffsetInVertices += cmdList.VtxBuffer.Size;
                indexOffsetInElements += cmdList.IdxBuffer.Size;
            }
            GL.BindBuffer(BufferTarget.ArrayBuffer, 0);
            GL.BindBuffer(BufferTarget.ElementArrayBuffer, 0);

            // Enable states
            GL.Enable(EnableCap.Blend);
            GL.Enable(EnableCap.ScissorTest);
            GL.Disable(EnableCap.CullFace);
            GL.Disable(EnableCap.DepthTest);

            GL.BlendEquation(BlendEquationMode.FuncAdd);
            GL.BlendFunc(BlendingFactor.SrcAlpha, BlendingFactor.OneMinusSrcAlpha);

            ImGuiIOPtr io = ImGui.GetIO();
            Matrix4 mvp = Matrix4.CreateOrthographicOffCenter(
                0f, io.DisplaySize.X, //width
                io.DisplaySize.Y, 0f, //height
                -1f, 1f); //near and far plane
            
            GL.UseProgram(shader);
            GL.UniformMatrix4(projectionLoc, true, ref mvp); //projection
            GL.Uniform1(textureLoc, 0);

            GL.BindVertexArray(meshVAO);

            drawData.ScaleClipRects(io.DisplayFramebufferScale);

            // Render command lists
            int vtxOffset = 0;
            int idxOffset = 0;
            for (int n = 0; n < drawData.CmdListsCount; n++)
            {
                ImDrawListPtr cmdList = drawData.CmdListsRange[n];
                for (int i = 0; i < cmdList.CmdBuffer.Size; i++)
                {
                    ImDrawCmdPtr pcmd = cmdList.CmdBuffer[i];

                    if (pcmd.UserCallback != IntPtr.Zero)
                        throw new NotImplementedException();

                    GL.ActiveTexture(TextureUnit.Texture0);
                    GL.BindTexture(TextureTarget.Texture2D, (int)pcmd.TextureId);

                    var clip = pcmd.ClipRect;
                    GL.Scissor((int)clip.X, windowHeight - (int)clip.W, (int)(clip.Z - clip.X), (int)(clip.W - clip.Y));

                    GL.DrawElementsBaseVertex(PrimitiveType.Triangles, (int)pcmd.ElemCount, DrawElementsType.UnsignedShort, (IntPtr)(idxOffset * sizeof(ushort)), vtxOffset);

                    idxOffset += (int)pcmd.ElemCount;
                }
                vtxOffset += cmdList.VtxBuffer.Size;
            }

            GL.ActiveTexture(TextureUnit.Texture0);
            GL.BindTexture(TextureTarget.Texture2D, 0);

            GL.Disable(EnableCap.Blend);
            GL.Disable(EnableCap.ScissorTest);

            GL.BindVertexArray(0);
            GL.UseProgram(0);
        }
    }
}
