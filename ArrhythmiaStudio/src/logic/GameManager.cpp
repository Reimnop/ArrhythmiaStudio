#include "GameManager.h"

#include <functional>

#include "VideoExporter.h"
#include "../Arguments.h"
#include "../engine/rendering/ImGuiController.h"
#include "../engine/rendering/Renderer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "object_behaviours/EmptyObjectBehaviour.h"
#include "object_behaviours/NormalObjectBehaviour.h"
#include "object_behaviours/TextObjectBehaviour.h"
#include "factories/FontFactory.h"
#include "factories/ObjectBehaviourFactory.h"
#include "factories/ShapeFactory.h"
#include "factories/LevelEventFactory.h"
#include "editor_windows/Timeline.h"
#include "editor_windows/Viewport.h"
#include "editor_windows/Properties.h"
#include "editor_windows/Events.h"
#include "editor_windows/Themes.h"
#include "level_events/CameraLevelEvent.h"
#include "level_events/BloomLevelEvent.h"
#include "object_behaviours/PrefabInstanceObjectBehaviour.h"

GameManager* GameManager::inst;

GameManager::GameManager(GLFWwindow* window)
{
	if (inst)
	{
		return;
	}
	inst = this;

	mainWindow = window;

	ImGuiController::onLayout += EventHandler<>([this] { onLayout(); });

	docManager = std::make_unique<DocumentationWindow>();
    prefabManager = std::make_unique<PrefabManagerWindow>();
    videoExporterWindow = std::make_unique<VideoExporterWindow>();

	Font::initFt();

	// Register fonts
	FontFactory::registerFont("Assets/Fonts/Inconsolata-Regular.ttf", "inconsolata", "Inconsolata");
	FontFactory::registerFont("Assets/Fonts/Anurati-Regular.otf", "anurati", "Anurati");

	// Register object behaviours
	ObjectBehaviourFactory::registerBehaviour<EmptyObjectBehaviour>("empty", "Empty");
	ObjectBehaviourFactory::registerBehaviour<NormalObjectBehaviour>("normal", "Normal");
	ObjectBehaviourFactory::registerBehaviour<TextObjectBehaviour>("text", "Text");
    ObjectBehaviourFactory::registerBehaviour<PrefabInstanceObjectBehaviour>("prefab_instance", "Prefab instance");

	// Register level events
	LevelEventFactory::registerEvent<CameraLevelEvent>("camera", "Camera");
	LevelEventFactory::registerEvent<BloomLevelEvent>("bloom", "Bloom");

	// Register shapes
	ShapeFactory::registerShape("Assets/Shapes/square.shp", "square");
	ShapeFactory::registerShape("Assets/Shapes/hollow_square.shp", "hollow_square");
	ShapeFactory::registerShape("Assets/Shapes/circle.shp", "circle");
	ShapeFactory::registerShape("Assets/Shapes/circle_quarter.shp", "circle_quarter");
	ShapeFactory::registerShape("Assets/Shapes/hollow_circle.shp", "hollow_circle");
	ShapeFactory::registerShape("Assets/Shapes/hollow_circle_quarter.shp", "hollow_circle_quarter");
	ShapeFactory::registerShape("Assets/Shapes/pentagon.shp", "pentagon");
	ShapeFactory::registerShape("Assets/Shapes/hollow_pentagon.shp", "hollow_pentagon");
	ShapeFactory::registerShape("Assets/Shapes/hexagon.shp", "hexagon");
	ShapeFactory::registerShape("Assets/Shapes/hollow_hexagon.shp", "hollow_hexagon");
	ShapeFactory::registerShape("Assets/Shapes/triangle.shp", "triangle");
	ShapeFactory::registerShape("Assets/Shapes/right_angled_triangle.shp", "right_angled_triangle");

	Level::initEvents();

	// Initialize level based on arguments
	std::string str;
	if (Arguments::tryGet("new", &str))
	{
		std::string levelPath = Arguments::get("level-path");
		std::string songPath = Arguments::get("song-path");
		level = std::make_unique<Level>(str, songPath, levelPath);
	}
	else if (Arguments::tryGet("open", &str))
	{
		level = std::make_unique<Level>(str);
	}
	else
	{
		LOG4CXX_ERROR(logger, "Level not specified!");
		std::exit(EXIT_FAILURE);
	}

	editorWindows.push_back(std::make_unique<Viewport>());
	editorWindows.push_back(std::make_unique<Timeline>());
	editorWindows.push_back(std::make_unique<Properties>());
	editorWindows.push_back(std::make_unique<Events>());
	editorWindows.push_back(std::make_unique<Themes>());
}

void GameManager::update()
{
	level->update();
}

void GameManager::onLayout()
{
	ImGui::SetNextWindowSize(ImVec2(320.0f, 100.0f));
	if (ImGui::BeginPopupModal("About", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings))
	{
		ImGui::TextWrapped("This software uses libraries from the FFmpeg project under the LGPLv2.1");
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	bool doAboutPopup = false;

	// Menu bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				level->save();
			}

			if (ImGui::MenuItem("Export to Video"))
			{
				videoExporterWindow->isOpen = true;
			}

			ImGui::EndMenu();
		}
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Prefabs"))
            {
                prefabManager->isOpen = true;
            }

            ImGui::EndMenu();
        }
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Documentation"))
			{
				docManager->isOpen = true;
			}

			if (ImGui::MenuItem("About"))
			{
				doAboutPopup = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (doAboutPopup)
	{
		ImGui::OpenPopup("About");
	}

	for (std::unique_ptr<EditorWindow>& window : editorWindows)
	{
		if (ImGui::Begin(window->getTitle().c_str())) 
		{
			if (!ImGui::GetCurrentWindow()->SkipItems) 
			{
				window->draw();
			}
		}
		ImGui::End();
	}
}