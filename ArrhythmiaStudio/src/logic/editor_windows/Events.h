#pragma once

#include "EditorWindow.h"

class Events : public EditorWindow
{
public:
	~Events() override = default;

	std::string getTitle() override;
	void draw() override;
};