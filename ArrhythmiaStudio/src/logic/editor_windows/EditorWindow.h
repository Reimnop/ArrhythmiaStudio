#pragma once

class EditorWindow
{
public:
	virtual ~EditorWindow() = default;

	virtual void draw() = 0;
};