#include "PrefabManagerWindow.h"
#include "../engine/rendering/ImGuiController.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "Level.h"

PrefabManagerWindow::PrefabManagerWindow()
{
    ImGuiController::onLayout += EventHandler<>([this] { onLayout(); });
}

void PrefabManagerWindow::onLayout()
{
    if (isOpen)
    {
        if (ImGui::Begin("Prefab manager", &isOpen))
        {
            Level& level = *Level::inst;

            if (ImGui::BeginChild("prefabs-list", ImVec2(200.0f, 0.0f), true))
            {
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
                {
                    selectedPrefab.reset();
                }

                for (auto &[id, prefab] : level.prefabs)
                {
                    if (ImGui::Selectable(prefab->name.c_str(),
                                          selectedPrefab.has_value() && selectedPrefab.value()->id == id))
                    {
                        selectedPrefab = prefab;
                    }
                }

                ImGui::EndChild();
            }

            ImGui::SameLine();

            if (ImGui::BeginChild("prefab-settings"))
            {
                if (selectedPrefab.has_value())
                {
                    Prefab* prefab = selectedPrefab.value();

                    ImGui::InputText("Name", &prefab->name);
                }
                else
                {
                    ImGui::Text("No prefab selected");
                }

                ImGui::EndChild();
            }
        }
        ImGui::End();
    }
}