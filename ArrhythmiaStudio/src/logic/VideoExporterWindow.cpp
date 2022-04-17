#include "VideoExporterWindow.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "Level.h"
#include "VideoExporter.h"
#include "../engine/rendering/ImGuiController.h"

VideoExporterWindow::VideoExporterWindow()
{
    ImGuiController::onLayout += EventHandler<>([this] { onLayout(); });
}

void VideoExporterWindow::onLayout()
{
    if (isOpen)
    {
        if (ImGui::Begin("Export video", &isOpen))
        {
            ImGui::InputText("Path", &path);
            ImGui::DragInt("Framerate", &framerate, 1, 1, 60);
            ImGui::DragInt("Width", &width, 1, 16, 8192);
            ImGui::DragInt("Height", &height, 1, 16, 4352);
            ImGui::DragFloat("Start time", &start, 0.2f, 0.0f, Level::inst->levelLength);
            ImGui::DragFloat("End time", &end, 0.2f, start, Level::inst->levelLength);

            if (ImGui::Button("Render"))
            {
                VideoExporter exporter = VideoExporter(width, height, framerate, start * framerate, end * framerate, "libvpx-vp9", "aac");
                exporter.exportToVideo(path);
            }
        }
        ImGui::End();
    }
}