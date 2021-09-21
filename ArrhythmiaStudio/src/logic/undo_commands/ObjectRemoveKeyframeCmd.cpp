#include "ObjectRemoveKeyframeCmd.h"

ObjectRemoveKeyframeCmd::ObjectRemoveKeyframeCmd(const uint64_t objectId, const AnimationChannelType type, const Keyframe kf)
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