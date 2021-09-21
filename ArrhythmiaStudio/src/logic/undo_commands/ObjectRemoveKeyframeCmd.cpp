#include "ObjectRemoveKeyframeCmd.h"

ObjectRemoveKeyframeCmd::ObjectRemoveKeyframeCmd(const LevelObject* obj, const AnimationChannelType type, const Keyframe kf)
{
	addKfCmd = std::make_unique<ObjectAddKeyframeCmd>(obj, type, kf);
}

void ObjectRemoveKeyframeCmd::undo()
{
	addKfCmd->redo();
}

void ObjectRemoveKeyframeCmd::redo()
{
	addKfCmd->undo();
}