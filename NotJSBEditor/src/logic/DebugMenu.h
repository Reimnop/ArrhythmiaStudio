#pragma once

class DebugMenu
{
public:
	DebugMenu();
private:
	bool open = false;
	int interval = 1;

	int lastFramerate = 0;
	int framerate = 0;
	float lastTime = 0.0f;

	void onLayout();
};
