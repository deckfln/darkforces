#pragma once

#include <map>
#include <string>
#include "fwAnimationKeyframe.h"
#include "fwBoneInfo.h"

class fwAnimation {
	std::string m_name;
	double m_Duration;
	fwBoneInfo* m_skeleton;
	std::map<double, fwAnimationKeyframe*> m_keyframes;

public:
	fwAnimation(std::string name, double duration, fwBoneInfo* skeleton);
	fwAnimationKeyframe* keyframes(double time);
	~fwAnimation();
};