#pragma once

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

#include "../framework/math/fwCylinder.h"

const float c_gravity = -0.00000981f;

class dfLevel;

class gaActor
{
	fwCylinder m_bounding = fwCylinder();		// player bounding cylinder
	glm::vec3 m_direction = glm::vec3(0, c_gravity, 0);	// normalized front facing vector
	glm::vec3 m_velocity = glm::vec3(0);

	float m_speed = 0.0015f;					// normal speed
	float m_run = 0.0030f;						// speed up when running

	float m_ankle = 0;							// maximum step the actor can walk up
	float m_eyes = 0;							// position of the eyes (from the feet)

	glm::mat3x3 m_physic=glm::mat3x3(0);
	time_t m_time = 0;							// start of the physic driven movement

	dfLevel* m_level = nullptr;

public:
	gaActor(fwCylinder& bounding, float eyes, float ankle);
	bool moveTo(time_t delta, bool run);
	void rotate(const glm::vec3& direction);
	const glm::vec3& position(void) { return m_bounding.position(); };
	float height(void);
	float radius(void);
	float eyes(void) { return m_eyes; };
	void jump(void);
	void bind(dfLevel* level) { m_level = level; };
	~gaActor();
};