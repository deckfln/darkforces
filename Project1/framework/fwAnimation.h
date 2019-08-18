#pragma once

#include <map>
#include <string>
#include "fwAnimationKeyframe.h"
#include "fwBoneInfo.h"

class fwAnimation {
	std::string m_name;
	double m_Duration;
	fwBoneInfo* m_skeleton;

	time_t m_startAt = 0;

public:
	fwAnimation(std::string name, double duration, fwBoneInfo* skeleton);

	std::string& name(void) { return m_name; };

	void reset(void);
	void update(glm::mat4 *target);

	~fwAnimation();
};