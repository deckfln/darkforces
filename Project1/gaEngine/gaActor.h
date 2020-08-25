#pragma once

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

#include "../framework/fwAABBox.h"
#include "../framework/math/fwCylinder.h"

#include "gaEntity.h"

const float c_gravity = -0.00000981f;
const float c_jump = -c_gravity * 300.0f;

class dfLevel;
class gaPlayer;

class gaActor: public gaEntity
{
	fwCylinder m_cylinder;						// player bounding cylinder
	GameEngine::Transform m_transforms;		// transforms to move the object

	float m_speed = 0.0480f;					// normal speed

	float m_ankle = 0;							// maximum step the actor can walk up
	float m_eyes = 0;							// position of the eyes (from the feet)

	glm::mat3x3 m_physic=glm::mat3x3(0);
	time_t m_animation_time = 0;							// start of the physic driven movement

	dfLevel* m_level = nullptr;
	gaPlayer* m_parent = nullptr;			// parent player

public:
	gaActor(
		int mclass,
		const std::string& name,		// name of the actor
		fwCylinder& cylinder,			// collision cylinder
		const glm::vec3& feet,			// position of the feet in world space
		float eyes,						// distance from the feet to the eyes (camera view)
		float ankle						// distance from the feet to the ankles (can step over)
		);
	bool moveTo(time_t delta, glm::vec3& velocity);
	bool moveTo_old(time_t delta, glm::vec3& velocity);
	void rotate(const glm::vec3& direction);
	float height(void);
	float radius(void);
	float eyes(void) { return m_eyes; };
	void jump(const glm::vec3& velocity);
	void bind(dfLevel* level) { m_level = level; };
	void parent(gaPlayer* parent) { m_parent = parent; };

	void dispatchMessage(gaMessage* message) override;

	~gaActor();
};