#pragma once

#include <glm/vec3.hpp>

class alListener
{
	glm::vec3 m_position;
	float m_maxdistance = 100.0f;								// maximum distance to hear a sound
	float m_mindistance = 0.0f;									// maximum distance to hear a sound

public:
	alListener();

	// setter/getter
	inline void maxdistance(float m) { m_maxdistance = m; };	// set attenuation model
	inline float maxdistance(void) { return m_maxdistance; };
	inline void mindistance(float m) { m_mindistance = m; };	// set attenuation model
	inline float mindistance(void) { return m_mindistance; };

	void position(const glm::vec3& position);
	void orientation(const glm::vec3& at, const glm::vec3& up);
	bool clamp(const glm::vec3& p);								// sound gets clamped after the max distance
	~alListener();
};
extern alListener g_Listener;
