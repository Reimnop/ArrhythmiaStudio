#pragma once
#include "EditorWindow.h"

class Timeline : public EditorWindow
{
public:
	~Timeline() override = default;

	void draw() override;
private:
	void drawTimeline();
};
