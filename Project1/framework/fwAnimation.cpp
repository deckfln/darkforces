#include "fwAnimation.h"

fwAnimation::fwAnimation(std::string name, double duration, fwBoneInfo* skeleton) :
	m_name(name),
	m_Duration(duration),
	m_skeleton(skeleton)
{

}

fwAnimationKeyframe *fwAnimation::keyframes(double time)
{
	fwAnimationKeyframe* keyframe = m_keyframes[time];
	if (keyframe == nullptr) {
		keyframe = m_keyframes[time] = new fwAnimationKeyframe(time, m_skeleton);
	}

	return keyframe;
}

fwAnimation::~fwAnimation()
{

}