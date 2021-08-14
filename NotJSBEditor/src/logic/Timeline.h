#pragma once

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
};
