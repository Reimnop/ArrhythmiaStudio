#pragma once

struct LevelObjectProperties
{
	float startTime;
	float killTime;
	bool isText;
	std::string text;
	int shapeIndex;
	int editorBinIndex;
	int colorSlotIndex;
	float depth;
	int layer;
};