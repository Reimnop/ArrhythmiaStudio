#include "DataManager.h"

#include "LevelManager.h"

#include <fstream>
#include <sstream>
#include <winnt.h>
#include <Windows.h>
#include <nlohmann/json.hpp>

DataManager* DataManager::inst;

DataManager::DataManager()
{
	if (inst)
	{
		return;
	}

	inst = this;
}


void DataManager::saveLevel(bool saveAs)
{
	WCHAR fileName[MAX_PATH];
	ZeroMemory(&fileName, MAX_PATH);
	fileName[0] = '\0';

	OPENFILENAMEW ofn;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = TEXT("NotJSBEditor Level File\0*.njelv\0");
	ofn.lpstrCustomFilter = NULL;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = TEXT("Save Level File...");
	ofn.Flags = OFN_EXPLORER;
	ofn.lpstrDefExt = TEXT("");

	if (GetSaveFileNameW(&ofn))
	{
		std::ofstream s(fileName);
		s << LevelManager::inst->level->toJson();

		s.close();
	}
}

void DataManager::openLevel()
{
	WCHAR fileName[MAX_PATH];
	ZeroMemory(&fileName, MAX_PATH);
	fileName[0] = '\0';

	OPENFILENAMEW ofn;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = TEXT("NotJSBEditor Level File\0*.njelv\0");
	ofn.lpstrCustomFilter = NULL;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = TEXT("Select Level File...");
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = TEXT("");

	if (GetOpenFileNameW(&ofn))
	{
		std::ifstream s(fileName);

		std::stringstream ss;
		ss << s.rdbuf();
		nlohmann::json j = nlohmann::json::parse(ss.str());
		s.close();

		LevelManager::inst->loadLevel(j);
	}
}
