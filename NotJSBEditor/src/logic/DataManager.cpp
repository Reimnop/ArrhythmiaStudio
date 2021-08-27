#include "DataManager.h"

#include "LevelManager.h"

#include <fstream>
#include <filesystem>
#include <shobjidl.h>
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

void DataManager::newLevel(LevelCreateInfo createInfo)
{
	levelDir = createInfo.levelPath;

	genLevelFiles(createInfo);
	LevelManager::inst->loadLevel(createInfo.levelPath);

	isStartupLevel = false;
}

void DataManager::saveLevel(bool saveAs)
{
	if (saveAs || levelDir.empty())
	{
		IFileDialog* fd;
		CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fd));

		DWORD dwFlags;
		fd->GetOptions(&dwFlags);
		fd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | FOS_PICKFOLDERS);
		fd->Show(NULL);

		IShellItem* psiResult;
		if (SUCCEEDED(fd->GetResult(&psiResult)))
		{
			PWSTR pszFilePath = NULL;
			psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

			std::wstring ws(pszFilePath);
			std::string newLevelDir(ws.begin(), ws.end());

			std::filesystem::copy(levelDir, newLevelDir);
			levelDir = newLevelDir;

			CoTaskMemFree(pszFilePath);
		}

		fd->Release();
	}

	std::filesystem::path levelFilePath(levelDir);
	levelFilePath /= "level.njelv";

	std::ofstream s(levelFilePath);
	s << LevelManager::inst->level->toJson();
	s.close();
}

void DataManager::openLevel()
{
	IFileDialog* fd;
	CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fd));

	DWORD dwFlags;
	fd->GetOptions(&dwFlags);
	fd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM | FOS_PICKFOLDERS);
	fd->Show(NULL);

	IShellItem* psiResult;
	if (SUCCEEDED(fd->GetResult(&psiResult)))
	{
		PWSTR pszFilePath = NULL;
		psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

		std::wstring ws(pszFilePath);

		levelDir = std::string(ws.begin(), ws.end());
		LevelManager::inst->loadLevel(levelDir);

		CoTaskMemFree(pszFilePath);

		isStartupLevel = false;
	}

	fd->Release();
}

void DataManager::genLevelFiles(LevelCreateInfo createInfo) const
{
	nlohmann::ordered_json j;
	j["name"] = createInfo.levelName;
	j["objects"] = nlohmann::ordered_json::array();

	for (int i = 0; i < 30; i++)
	{
		j["color_slots"][i]["keyframes"][0]["time"] = 0.0f;
		j["color_slots"][i]["keyframes"][0]["color"][0] = 1.0f;
		j["color_slots"][i]["keyframes"][0]["color"][1] = 1.0f;
		j["color_slots"][i]["keyframes"][0]["color"][2] = 1.0f;
	}

	std::filesystem::path levelFilePath(createInfo.levelPath);
	levelFilePath /= "level.njelv";
	std::filesystem::path songFilePath(createInfo.levelPath);
	songFilePath /= "song.ogg";

	std::ofstream s(levelFilePath);
	s << j;
	s.close();

	std::filesystem::copy(createInfo.levelSong, songFilePath);
}
