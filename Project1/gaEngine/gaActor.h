#pragma once

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

#include "../framework/fwAABBox.h"
#include "../framework/math/fwCylinder.h"

#include "gaEntity.h"

const float c_gravity = -0.00000981f;
const float c_jump = -c_gravity * 300.0f;

class dfLevel;

class gaActor: public gaEntity
{
	fwCylinder m_bounding;						// player bounding cylinder
	glm::vec3 m_direction = glm::vec3(0, c_gravity, 0);	// normalized front facing vector
	glm::vec3 m_velocity = glm::vec3(0);

	float m_speed = 0.0480f;					// normal speed

	float m_ankle = 0;							// maximum step the actor can walk up
	float m_eyes = 0;							// position of the eyes (from the feet)

	glm::mat3x3 m_physic=glm::mat3x3(0);
	time_t m_time = 0;							// start of the physic driven movement

	dfLevel* m_level = nullptr;

public:
	gaActor(const std::string& name, fwCylinder& bounding, float eyes, float ankle);
	bool moveTo(time_t delta, glm::vec3& velocity);
	void rotate(const glm::vec3& direction);
	const glm::vec3& position(void) { return m_bounding.position(); };
	float height(void);
	float radius(void);
	float eyes(void) { return m_eyes; };
	void jump(const glm::vec3& velocity);
	void bind(dfLevel* level) { m_level = level; };
	~gaActor();
};