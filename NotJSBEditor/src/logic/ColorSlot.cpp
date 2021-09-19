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

	sequence = new ColorSequence(count, keyframes);
	material = new Material(shader, 1, properties);

	for (int i = 0; i < count; i++)
	{
		
	}

	update(0.0f);
}

ColorSlot::ColorSlot(nlohmann::json j)
{
	MaterialProperty properties[]
	{
		MaterialProperty("Color", MaterialPropertyType_Vector3, 12)
	};

	sequence = new ColorSequence(0, nullptr);
	for (const nlohmann::json& kfJson : j["keyframes"])
	{
		insertKeyframe(ColorKeyframe(kfJson));
	}

	material = new Material(shader, 1, properties);

	update(0.0f);
}

ColorSlot::~ColorSlot()
{
	delete sequence;
	delete material;
}


void ColorSlot::insertKeyframe(const ColorKeyframe& kf)
{
	keyframes.push_back(kf);
	sequence->insertKeyframe(kf);
}

void ColorSlot::eraseKeyframe(const ColorKeyframe& kf)
{
	const std::vector<ColorKeyframe>::iterator it = std::remove(keyframes.begin(), keyframes.end(), kf);
	keyframes.erase(it);

	sequence->eraseKeyframe(kf);
}

void ColorSlot::update(float time)
{
	currentColor = sequence->update(time);
	material->setVec3("Color", glm::vec3(currentColor.r, currentColor.g, currentColor.b));
	material->updateBuffer();
}

nlohmann::json ColorSlot::toJson() const
{
	nlohmann::json j;
	j["keyframes"] = nlohmann::json::array();
	for (const ColorKeyframe& kf : keyframes)
	{
		j["keyframes"].push_back((const nlohmann::json)kf.toJson());
	}

	return j;
}
