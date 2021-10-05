#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <imgui/imgui.h>
#include <imgui/imgui_markdown.h>

#include "DocPage.h"
#include "../rendering/Texture2D.h"

class DocManager
{
public:
	static DocManager* inst;

	bool isOpen = false;

	DocManager();
private:
	ImFont* h1Fnt;
	ImFont* h2Fnt;
	ImFont* h3Fnt;

	ImGui::MarkdownConfig mdConfig;

	int selectedPageIndex = 0;
	std::vector<DocPage> pages;

	std::unordered_map<std::string, Texture2D*> images;

	void onLayout();
	void markdown(std::string str);

	DocPage pageFromFile(std::filesystem::path path);

	void linkCallback(ImGui::MarkdownLinkCallbackData data);
	ImGui::MarkdownImageData imageCallback(ImGui::MarkdownLinkCallbackData data);
	void formatCallback(const ImGui::MarkdownFormatInfo& markdownFormatInfo, bool start);
};
