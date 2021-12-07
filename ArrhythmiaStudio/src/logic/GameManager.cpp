#include "GameManager.h"

#include <functional>

#include "logger.h"
#include "../engine/rendering/ImGuiController.h"
#include "../engine/rendering/Renderer.h"
#include "imgui/imgui.h"
#include "imgui/imgui_markdown.h"
#include "object_behaviours/NormalObjectBehaviour.h"
#include "factories/ObjectBehaviourFactory.h"
#include "factories/ShapeFactory.h"
#include "factories/LevelEventFactory.h"
#include "editor_windows/Timeline.h"
#include "editor_windows/Viewport.h"
#include "editor_windows/Properties.h"
#include "editor_windows/Events.h"
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

	// TODO: remove dummy objects generator
	json j = json::parse("{\"objects\":[{\"name\":\"object index 61\",\"type\":\"normal\",\"id\":3320485139699489000,\"start\":1.524999976158142,\"end\":6.525000095367432,\"shape\":\"square\",\"px\":[[0,0,1]],\"py\":[[0,0,1]],\"sx\":[[0,1,1]],\"sy\":[[0,1,1]],\"ro\":[[0,0,1]],\"op\":[[0,1,1]]}],\"events\":[{\"type\":\"camera\",\"x\":[[0,0,1]],\"y\":[[0,0,1]],\"r\":[[0,0,1]],\"s\":[[0,10,1]]}]}");
	level = new Level(j);
	
	/* for (int i = 0; i < 100; i++)
	{
		float time = i * 0.025f;
		LevelObject* object = new LevelObject("normal", level);
		object->startTime = time;
		object->endTime = time + 5.0f;
		object->bin = i % 15;
		object->setName("object index " + std::to_string(i));

		level->insertObject(object);
		level->insertActivateList(object);
		level->insertDeactivateList(object);
	}

	level->recalculateObjectsState(); */

	editorWindows.push_back(new Viewport());
	editorWindows.push_back(new Timeline());
	editorWindows.push_back(new Properties());
	editorWindows.push_back(new Events());
}

void GameManager::update()
{

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
			window->draw();
		}
		ImGui::End();
	}
}