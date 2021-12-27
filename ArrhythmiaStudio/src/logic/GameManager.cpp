#include "GameManager.h"

#include <functional>

#include "../Arguments.h"
#include "../engine/rendering/ImGuiController.h"
#include "../engine/rendering/Renderer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_markdown.h"
#include "imgui/imgui_internal.h"
#include "object_behaviours/NormalObjectBehaviour.h"
#include "object_behaviours/TextObjectBehaviour.h"
#include "factories/ObjectBehaviourFactory.h"
#include "factories/ShapeFactory.h"
#include "factories/LevelEventFactory.h"
#include "editor_windows/Timeline.h"
#include "editor_windows/Viewport.h"
#include "editor_windows/Properties.h"
#include "editor_windows/Events.h"
#include "editor_windows/Themes.h"
#include "level_events/CameraLevelEvent.h"

#define WELCOME_MSG Welcome to PROJECT_NAME!

GameManager* GameManager::inst;

GameManager::GameManager(GLFWwindow* window)
{
	if (inst)
	{
		return;
	}
	inst = this;

	mainWindow = window;

	ImGuiController::onLayout.push_back(std::bind(&GameManager::onLayout, this));

	discordManager = new DiscordManager();
	docManager = new DocManager();

	// Register object behaviours
	ObjectBehaviourFactory::registerBehaviour<NormalObjectBehaviour>("normal", "Normal");
	ObjectBehaviourFactory::registerBehaviour<TextObjectBehaviour>("text", "Text");

	// Register level events
	LevelEventFactory::registerEvent<CameraLevelEvent>("camera", "Camera");

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

	// Initialize level based on arguments
	std::string str;
	if (Arguments::tryGet("new", &str))
	{
		std::string levelPath = Arguments::get("level-path");
		std::string songPath = Arguments::get("song-path");
		level = new Level(str, songPath, levelPath);
	}
	else if (Arguments::tryGet("open", &str))
	{
		level = new Level(str);
	}
	else
	{
		LOG4CXX_ERROR(logger, "Level not specified!");
		std::exit(EXIT_FAILURE);
	}

	editorWindows.push_back(new Viewport());
	editorWindows.push_back(new Timeline());
	editorWindows.push_back(new Properties());
	editorWindows.push_back(new Events());
	editorWindows.push_back(new Themes());
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

	for (EditorWindow* window : editorWindows)
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