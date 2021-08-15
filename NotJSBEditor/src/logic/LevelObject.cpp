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
	colorSlotIndex = j["color_slot"].get<int>();
	editorBinIndex = j["editor_bin"].get<int>();

	nlohmann::json::array_t arr = j["channels"].get<nlohmann::json::array_t>();
	for (int i = 0; i < arr.size(); i++)
	{
		animationChannels.push_back(new AnimationChannel(arr[i]));
	}

	node = nullptr;
}

LevelObject::~LevelObject()
{
	for (AnimationChannel* channel : animationChannels)
	{
		delete channel;
	}

	animationChannels.clear();
	animationChannels.shrink_to_fit();
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
	if (animationChannels.size() == 0)
	{
		animationChannels.push_back(value);
		return;
	}

	std::vector<AnimationChannel*>::iterator it = std::lower_bound(animationChannels.begin(), animationChannels.end(),
	                                                               value,
	                                                               [](AnimationChannel* a, AnimationChannel* b)
	                                                               {
		                                                               return a->type < b->type;
	                                                               });
	animationChannels.insert(it, value);
}

bool LevelObject::hasChannel(AnimationChannelType channelType)
{
	for (int i = 0; i < animationChannels.size(); i++)
	{
		if (animationChannels[i]->type == channelType)
		{
			return true;
		}
	}

	return false;
}

nlohmann::ordered_json LevelObject::toJson()
{
	nlohmann::ordered_json j;
	j["name"] = name;
	j["start"] = startTime;
	j["kill"] = killTime;
	j["color_slot"] = colorSlotIndex;
	j["editor_bin"] = editorBinIndex;

	j["channels"] = nlohmann::json::array();
	for (int i = 0; i < animationChannels.size(); i++)
	{
		j["channels"][i] = animationChannels[i]->toJson();
	}

	return j;
}
