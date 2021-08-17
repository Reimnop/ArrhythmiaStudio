#include "ColorSlot.h"

Shader* ColorSlot::shader;

void ColorSlot::init()
{
	shader = new Shader("Assets/Shaders/unlit.vert", "Assets/Shaders/unlit.frag");
}

ColorSlot::ColorSlot(int count, ColorKeyframe* keyframes)
{
	MaterialProperty properties[]
	{
		MaterialProperty("Color", MaterialPropertyType_Vector3, 12)
	};

	channel = new ColorChannel(count, keyframes);
	material = new Material(shader, 1, properties);

	update(0.0f);
}

ColorSlot::ColorSlot(nlohmann::json j)
{
	MaterialProperty properties[]
	{
		MaterialProperty("Color", MaterialPropertyType_Vector3, 12)
	};

	channel = new ColorChannel(j);
	material = new Material(shader, 1, properties);

	update(0.0f);
}

ColorSlot::~ColorSlot()
{
	delete channel;
	delete material;
}

void ColorSlot::update(float time)
{
	currentColor = channel->update(time);
	material->setVec3("Color", glm::vec3(currentColor.r, currentColor.g, currentColor.b));
	material->updateBuffer();
}

nlohmann::ordered_json ColorSlot::toJson()
{
	return channel->toJson();
}
