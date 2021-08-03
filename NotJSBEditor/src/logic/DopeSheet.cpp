#include "DopeSheet.h"
#include "LevelManager.h"

DopeSheet::DopeSheet(LevelManager* levelManager) {
	this->levelManager = levelManager;

	ImGuiController::onLayout.push_back(std::bind(&DopeSheet::onLayout, this));
}

void DopeSheet::onLayout() {

}