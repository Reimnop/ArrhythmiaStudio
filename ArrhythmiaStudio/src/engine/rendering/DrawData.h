#pragma once
#include "drawers/Drawer.h"

enum DrawDataType
{
	DrawDataType_Batched,
	DrawDataType_Text
};


// Generic interface for draw data
struct DrawData
{
	virtual ~DrawData() = default;
	virtual DrawDataType getType() const = 0;
	virtual Drawer* getDrawer() = 0;
};