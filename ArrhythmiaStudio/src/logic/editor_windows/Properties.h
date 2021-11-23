#pragma once
#include "EditorWindow.h"

class Properties : public EditorWindow
{
public:
	~Properties() override = default;

	std::string getTitle() override;
	void draw() override;
};