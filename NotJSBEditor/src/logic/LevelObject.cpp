#include "LevelObject.h"

LevelObject::LevelObject()
{
	node = nullptr;
}

LevelObject::LevelObject(nlohmann::json j)
{
	name = j["name"].get<std::string>();
	startTime = j["start"].get<float>();
	killTime = j["kill"].get<float>();
	depth = j["depth"].get<float>();
	shapeIndex = j["shape"].get<int>();
	colorSlotIndex = j["color_slot"].get<int>();
	editorBinIndex = j["editor_bin"].get<int>();

	nlohmann::json::array_t arr = j["channels"].get<nlohmann::json::array_t>();
	for (int i = 0; i < arr.size(); i++)
	{
		insertChannel(new AnimationChannel(arr[i]));
	}

	node = nullptr;
}

LevelObject::~LevelObject()
{
	// Unparent all children
	for (LevelObject* child : children)
	{
		child->setParent(nullptr);
	}

	for (AnimationChannel* channel : animationChannels)
	{
		delete channel;
	}

	delete node;

	animationChannels.clear();
	animationChannels.shrink_to_fit();
}

void LevelObject::setParent(LevelObject* newParent)
{
	// Remove from old parent
	if (parent)
	{
		std::vector<LevelObject*>::iterator it = std::remove(parent->children.begin(), parent->children.end(), this);
		parent->children.erase(it);
	}

	// Add to new parent
	if (newParent)
	{
		newParent->children.push_back(this);
		node->setParent(newParent->node);
	}
	else
	{
		node->setParent(nullptr);
	}

	parent = newParent;
}

void LevelObject::genActionPair(ObjectAction* spawnAction, ObjectAction* killAction)
{
	ObjectAction spawn = ObjectAction();
	spawn.time = startTime;
	spawn.type = ObjectActionType_Spawn;
	spawn.levelObject = this;

	ObjectAction kill = ObjectAction();
	kill.time = killTime + 0.0001f;
	kill.type = ObjectActionType_Kill;
	kill.levelObject = this;

	*spawnAction = spawn;
	*killAction = kill;
}

void LevelObject::insertChannel(AnimationChannel* value)
{
	if (hasChannel(value->type))
	{
		return;
	}

	std::vector<AnimationChannel*>::iterator it = std::lower_bound(animationChannels.begin(), animationChannels.end(),
	                                                               value,
	                                                               [](const AnimationChannel* a,
	                                                                  const AnimationChannel* b)
	                                                               {
		                                                               return a->type < b->type;
	                                                               });
	animationChannels.insert(it, value);

	animationChannelLookup[value->type] = true;
}

bool LevelObject::hasChannel(AnimationChannelType channelType)
{
	return animationChannelLookup[channelType];
}

nlohmann::ordered_json LevelObject::toJson(bool excludeChildren)
{
	nlohmann::ordered_json j;
	j["name"] = name;
	j["start"] = startTime;
	j["kill"] = killTime;
	j["depth"] = depth;
	j["shape"] = shapeIndex;
	j["color_slot"] = colorSlotIndex;
	j["editor_bin"] = editorBinIndex;

	j["channels"] = nlohmann::json::array();
	for (int i = 0; i < animationChannels.size(); i++)
	{
		j["channels"][i] = animationChannels[i]->toJson();
	}

	if (!excludeChildren)
	{
		j["children"] = nlohmann::json::array();
		for (int i = 0; i < children.size(); i++)
		{
			j["children"][i] = children[i]->toJson();
		}
	}

	return j;
}
