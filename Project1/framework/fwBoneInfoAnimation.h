#pragma once

#include "fwBoneInfo.h"
#include "fwAnimationKeyframe.h"

class fwBoneInfoAnimation
{
	fwBoneInfo* m_bone;

	std::list <fwBoneInfoAnimation*> m_children;

	std::map <time_t, fwAnimationKeyframe*> m_keyframes;

public:
	fwBoneInfoAnimation(fwBoneInfo* bone);

	fwAnimationKeyframe* keyframes(time_t time);
	void interpolate(time_t start, time_t end, float inbetween_t, glm::mat4* target, glm::mat4& parent, glm::mat4& GlobalInverseTransform);
	fwBoneInfoAnimation* bone(const std::string &name);

	~fwBoneInfoAnimation();
};