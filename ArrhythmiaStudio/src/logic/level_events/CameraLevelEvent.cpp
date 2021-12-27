#include "CameraLevelEvent.h"

#include "../../engine/rendering/Renderer.h"

CameraLevelEvent::CameraLevelEvent(Level* level) : AnimateableLevelEvent(level)
{
	Keyframe kf0 = Keyframe(0.0f, 0.0f, EaseType_Linear);
	Keyframe kf10 = Keyframe(0.0f, 10.0f, EaseType_Linear);
	positionX = Sequence(1, &kf0);
	positionY = Sequence(1, &kf0);
	rotation = Sequence(1, &kf0);
	scale = Sequence(1, &kf10);
}

LevelEvent* CameraLevelEvent::create(Level* level)
{
	return new CameraLevelEvent(level);
}

std::string CameraLevelEvent::getTitle()
{
	return "Camera";
}

void CameraLevelEvent::update(float time)
{
	Camera* camera = Renderer::inst->camera;
	camera->position = glm::vec3(
		positionX.update(time),
		positionY.update(time),
		0.0f);
	camera->rotation = angleAxis(
		rotation.update(time) / 180.0f * PI,
		glm::vec3(0.0f, 0.0f, -1.0f));
	camera->orthographicScale = scale.update(time);
}

void CameraLevelEvent::drawSequences()
{
	sequenceEdit(positionX, "Position X");
	sequenceEdit(positionY, "Position Y");
	sequenceEdit(rotation, "Rotation");
	sequenceEdit(scale, "Scale");
}

void CameraLevelEvent::readJson(json& j)
{
	positionX.fromJson(j["x"]);
	positionY.fromJson(j["y"]);
	rotation.fromJson(j["r"]);
	scale.fromJson(j["s"]);
}

void CameraLevelEvent::writeJson(json& j)
{
	j["x"] = positionX.toJson();
	j["y"] = positionY.toJson();
	j["r"] = rotation.toJson();
	j["s"] = scale.toJson();
}
