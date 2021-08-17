#pragma once

#include <string>

class LevelManager;

class Timeline
{
public:
	static Timeline* inst;

	Timeline();
private:
	float startTime;
	float endTime;

	void onLayout();
	bool playButton(bool playing);
	std::string timeToString(float time);
};
