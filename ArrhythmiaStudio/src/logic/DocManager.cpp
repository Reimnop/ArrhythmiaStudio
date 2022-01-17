#include "DocManager.h"
#include "../engine/rendering/ImGuiController.h"

#include <fstream>
#include <Windows.h>
#include <shellapi.h>
#include <helper.h>

DocManager* DocManager::inst;

DocManager::DocManager()
{
	if (inst)
	{
		return;
	}
	inst = this;

	// Read doc pages
	std::ifstream s("Assets/Documentation/doc_list.txt");

	std::string line;
	while (std::getline(s, line))
	{
		pages.push_back(pageFromFile("Assets/Documentation" / std::filesystem::path(line)));
	}

	s.close();

	// Load doc images
	for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator("Assets/Documentation/Images"))
	{
		const std::filesystem::path& path = entry.path();
		images.emplace(path.filename().generic_string(), new Texture2D(path));
	}

	// Init fonts
	ImGuiIO& io = ImGui::GetIO();

	h1Fnt = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Inconsolata-Bold.ttf", 24);
	h2Fnt = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Inconsolata-Bold.ttf", 21);
	h3Fnt = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Inconsolata-Bold.ttf", 18);

	mdConfig = ImGui::MarkdownConfig();
	mdConfig.linkCallback = cify([this](ImGui::MarkdownLinkCallbackData data) { linkCallback(data); });;
	mdConfig.imageCallback = cify([this](ImGui::MarkdownLinkCallbackData data) { return imageCallback(data); });
	mdConfig.formatCallback = cify([this](const ImGui::MarkdownFormatInfo& markdownFormatInfo, bool start) { formatCallback(markdownFormatInfo, start); });
	mdConfig.headingFormats[0] = { h1Fnt, true  };
	mdConfig.headingFormats[1] = { h2Fnt, true  };
	mdConfig.headingFormats[2] = { h3Fnt, false };

	ImGuiController::onLayout.push_back(std::bind(&DocManager::onLayout, this));
}

void DocManager::onLayout()
{
	if (isOpen) 
	{
		if (ImGui::Begin("Documentation", &isOpen))
		{
			if (ImGui::BeginChild("doc-pages", ImVec2(200.0f, 0.0f), true))
			{
				for (int i = 0; i < pages.size(); i++)
				{
					if (ImGui::Selectable(pages[i].name.c_str(), i == selectedPageIndex))
					{
						selectedPageIndex = i;
					}
				}

				ImGui::EndChild();
			}

			ImGui::SameLine();

			if (ImGui::BeginChild("doc-content"))
			{
				markdown(pages[selectedPageIndex].content);

				ImGui::EndChild();
			}

			ImGui::End();
		}
	}
}

void DocManager::markdown(std::string str)
{
	ImGui::Markdown(str.c_str(), str.length(), mdConfig);
}

DocPage DocManager::pageFromFile(std::filesystem::path path)
{
	std::ifstream s(path);

	std::string name;
	std::getline(s, name);

	std::stringstream ss;
	ss << s.rdbuf();

	DocPage page = DocPage();
	page.name = name;
	page.content = ss.str();

	s.close();

	return page;
}

void DocManager::linkCallback(ImGui::MarkdownLinkCallbackData data)
{
	std::string url(data.link, data.linkLength);
	if (!data.isImage)
	{
		ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}

ImGui::MarkdownImageData DocManager::imageCallback(ImGui::MarkdownLinkCallbackData data)
{
	std::string name(data.link, data.linkLength);

	ImGui::MarkdownImageData imageData;

	if (!images.count(name)) 
	{
		imageData.isValid = false;
		return imageData;
	}

	Texture2D* image = images[name];

	int w, h;
	image->getSize(&w, &h);

	imageData.isValid = true;
	imageData.useLinkCallback = false;
	imageData.user_texture_id = (ImTextureID)image->getHandle();
	imageData.size = ImVec2(w, h);

	return imageData;
}

void DocManager::formatCallback(const ImGui::MarkdownFormatInfo& markdownFormatInfo, bool start)
{
	ImGui::defaultMarkdownFormatCallback(markdownFormatInfo, start);

	if (markdownFormatInfo.type == ImGui::MarkdownFormatType::LINK)
	{
		if (start)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 0.6f, 1.0f, 1.0f)));
		}
		else
		{
			ImGui::PopStyleColor();
		}
	}
}
