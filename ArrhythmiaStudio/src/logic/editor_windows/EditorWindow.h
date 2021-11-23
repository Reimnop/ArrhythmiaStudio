#pragma once

#include <string>

class EditorWindow
{
public:
	virtual ~EditorWindow() = default;

	virtual std::string getTitle() = 0;
	virtual void draw() = 0;
};