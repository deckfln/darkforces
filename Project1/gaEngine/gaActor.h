#pragma once

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

#include "../framework/math/fwCylinder.h"

class dfLevel;

class gaActor
{
	glm::vec3 m_position = glm::vec3(0);		// position in GL space of the feet of the actor
	glm::vec3 m_direction = glm::vec3(1, 0, 0);	// front facing vector
	glm::vec3 m_velocity = glm::vec3(0);
	fwCylinder m_bounding = fwCylinder();		// player bounding cylinder
	float m_ankle = 0;							// maximum step the actor can walk up
	float m_eyes = 0;							// position of the eyes (from the feet)

	glm::mat3x3 m_physic=glm::mat3x3(0);
	time_t m_time = 0;							// start of the physic driven movement

	dfLevel* m_level = nullptr;

public:
	gaActor(const fwCylinder& bounding, const glm::vec3& position, float eyes, float ankle);
	void moveForward(float speed);
	void collision(time_t delta, glm::vec3& direction);
	~gaActor();
};