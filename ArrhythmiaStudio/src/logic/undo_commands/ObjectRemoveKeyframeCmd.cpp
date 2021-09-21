#include "ObjectRemoveKeyframeCmd.h"

ObjectRemoveKeyframeCmd::ObjectRemoveKeyframeCmd(uint64_t objectId, AnimationChannelType type, Keyframe kf)
{
	addKfCmd = std::make_unique<ObjectAddKeyframeCmd>(objectId, type, kf);
}

void ObjectRemoveKeyframeCmd::undo()
{
	addKfCmd->redo();
}

void ObjectRemoveKeyframeCmd::redo()
{
	addKfCmd->undo();
}