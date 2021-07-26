using System;
using System.Collections.Generic;
using System.Text;
using ImGuiNET;
using ImGuiOpenTK;
using OpenTK.Windowing.Desktop;

namespace NotJSBEditor
{
    public class Renderer
    {
        private GameWindow mainWindow;
        private ImGuiController imGuiController;
        
        public Renderer(GameWindow window)
        {
            // We set the mainWindow field to reference it later
            mainWindow = window;

            // Initialize rendering
            imGuiController = new ImGuiController(window, "Assets/Inconsolata.ttf");
        }

        // Should be run every frame
        // deltaTime: Time between last frame and this frame
        public void Update(float deltaTime)
        {
            imGuiController.Update(deltaTime);
        }

        // Renders entire scene
        public void RenderScene()
        {
            // Render ImGui last
            imGuiController.RenderImGui();
        }
    }
}
