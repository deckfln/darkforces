#include "fwAnimation.h"

#include <iostream>
#include <windows.h>
#include <vector>

fwAnimation::fwAnimation(std::string name, time_t duration, fwBoneInfoAnimation* skeleton) :
	m_name(name),
	m_Duration(duration),
	m_skeleton(skeleton)
{

}

void fwAnimation::reset(void)
{
	m_currentTime = m_keyframes[0];
}

void fwAnimation::keyframes(std::map<time_t, bool>& keyframes)
{
	
	m_nbKeyframes = keyframes.size();
	m_keyframes = new time_t[m_nbKeyframes]();

	int i = 0;

	for (auto const& keyframe : keyframes) {
		m_keyframes[i] = keyframe.first;
		i++;
	}
}

void fwAnimation::update(time_t elapsed, glm::mat4 *target, glm::mat4& GlobalInverseTransform)
{
	m_currentTime += elapsed;

	// find the interval
	glm::mat4 identity(1);

	time_t prev_t = m_keyframes[m_currentFrame];
	time_t next_t = m_keyframes[m_currentFrame + 1];

	while (m_currentTime >= next_t) {
		m_currentFrame++;

		if (m_currentFrame >= m_nbKeyframes - 1) {
			m_currentFrame = 0;
			m_currentTime = m_keyframes[0];

			prev_t = m_keyframes[0];
			next_t = m_keyframes[1];
		}
		else {
			prev_t = m_keyframes[m_currentFrame];
			next_t = m_keyframes[m_currentFrame + 1];
		}

		std::cout << m_currentFrame << ":" << m_currentTime << " ! " << std::endl;
	}

	time_t delta = (m_currentTime - prev_t);
	float inbetween_t = delta * 1.0 / (next_t - prev_t);
	/*
	std::string x = std::to_string(elapsed) + " " + std::to_string(m_currentTime) +" " + std::to_string(delta) + " " + std::to_string(prev_t) + " " + std::to_string(next_t) + " " + std::to_string(inbetween_t) + "\n";
	debug += x;
	*/
	// interpolate the fwAnimationKeyframes
	m_skeleton->interpolate(prev_t, next_t, inbetween_t, target, identity, GlobalInverseTransform);
}

fwAnimation::~fwAnimation()
{
	/*
	std::cout <<debug ;
	*/
	std::cout << "fwanimation:~fwAnimation" << std::endl;
	delete[] m_keyframes;
}