#include "BloomLevelEvent.h"

#include "../../engine/rendering/Renderer.h"

BloomLevelEvent::BloomLevelEvent(Level* level) : AnimateableLevelEvent(level)
{
	Keyframe kf0(0.0f, 0.0f, EaseType_Linear);
	Keyframe kf09(0.0f, 0.9f, EaseType_Linear);
	Keyframe kf08(0.0f, 0.8f, EaseType_Linear);
	intensity = Sequence(1, &kf0);
	scatter = Sequence(1, &kf09);
	threshold = Sequence(1, &kf08);
}

LevelEvent* BloomLevelEvent::create(Level* level)
{
	return new BloomLevelEvent(level);
}

std::string BloomLevelEvent::getTitle()
{
	return "Bloom";
}

void BloomLevelEvent::update(float time)
{
	Bloom& bloom = *Renderer::inst->bloom;
	bloom.intensity = intensity.update(time);
	bloom.scatter = scatter.update(time);
	bloom.threshold = threshold.update(time);
}

void BloomLevelEvent::drawSequences()
{
	sequenceEdit(intensity, "Intensity");
	sequenceEdit(scatter, "Scatter");
	sequenceEdit(threshold, "Threshold");
}

void BloomLevelEvent::readJson(json& j)
{
	intensity.fromJson(j["intensity"]);
	scatter.fromJson(j["scatter"]);
	threshold.fromJson(j["threshold"]);
}

void BloomLevelEvent::writeJson(json& j)
{
	j["intensity"] = intensity.toJson();
	j["scatter"] = scatter.toJson();
	j["threshold"] = threshold.toJson();
}
