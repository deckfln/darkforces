#pragma once

#include <map>
#include <string>

#include "fwBoneInfoAnimation.h"

class fwAnimation {
	std::string m_name;
	time_t m_Duration;
	fwBoneInfoAnimation* m_skeleton=nullptr;

	time_t m_currentTime = 0;
	int m_currentFrame = 0;

	int m_nbKeyframes = 0;
	time_t* m_keyframes = nullptr;

	std::string debug="";

public:
	fwAnimation(std::string name, time_t duration, fwBoneInfoAnimation* skeleton);

	std::string& name(void) { return m_name; };

	void keyframes(std::map<time_t, bool> &);

	void reset(void);
	void update(time_t delta, glm::mat4 *target, glm::mat4& GlobalInverseTransform);

	void skeleton(fwBoneInfoAnimation* root) { m_skeleton = root; };
	~fwAnimation();
};