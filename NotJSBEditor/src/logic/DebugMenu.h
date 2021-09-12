#pragma once

#include <string>

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

	std::string videoPath;
	int videoWidth = 1920;
	int videoHeight = 1080;
	int videoFramerate = 60;
	int videoStartFrame = 0;
	int videoEndFrame = 300;

	void onLayout();
};
