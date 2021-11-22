#pragma once

enum DrawDataType
{
	DrawDataType_Batched,
	DrawDataType_Text
};


// Generic interface for draw data
struct DrawData
{
	virtual DrawDataType getType() const = 0;
};