#include "fwAnimation.h"

#include <sysinfoapi.h>
#include <vector>

fwAnimation::fwAnimation(std::string name, double duration, fwBoneInfo* skeleton) :
	m_name(name),
	m_Duration(duration),
	m_skeleton(skeleton)
{

}

void fwAnimation::reset(void)
{
	m_startAt = GetTickCount64();
}

void fwAnimation::update(glm::mat4 *target)
{
	time_t now = GetTickCount64();
	time_t delta = m_startAt - now;

	if (delta > m_Duration) {
		delta = 0;
		m_startAt = GetTickCount64();
	}

	// find the interval
	std::vector<time_t> times;
	for (auto const& keyframe : m_keyframes) {
		times.push_back(keyframe.first);
	}

	for (auto i = 0; i < times.size() - 1; i++) {
		if (delta > times[i] && delta < times[i + 1]) {
			time_t prev_t = times[i];
			time_t next_t = times[i + 1];
			float inbetween_t = (delta - prev_t) * 1.0 / (next_t - prev_t);

			fwAnimationKeyframe* prev = m_keyframes[prev_t];
			fwAnimationKeyframe* next = m_keyframes[next_t];

			prev->interpolate(next, inbetween_t, target);
		}
	}
}

fwAnimation::~fwAnimation()
{
}