#pragma once

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

#include "../framework/fwAABBox.h"
#include "../framework/math/fwCylinder.h"

#include "gaEntity.h"
#include "Physics.h"

#include "../flightRecorder/Actor.h"

const float c_jump = -c_gravity * 300.0f;

class dfLevel;
class gaPlayer;

class gaActor: public gaEntity
{
	fwCylinder m_cylinder;						// player bounding cylinder

	float m_speed = 0.0480f;					// normal speed

	float m_ankle = 0;							// maximum step the actor can walk up
	float m_eyes = 0;							// position of the eyes (from the feet)
	float m_step = 0;							// how up/down can the actor step over

	glm::mat3x3 m_physic=glm::mat3x3(0);
	time_t m_animation_time = 0;				// start of the physic driven movement

	gaPlayer* m_parent = nullptr;				// parent player

public:
	gaActor(
		int mclass,
		const std::string& name,		// name of the actor
		fwCylinder& cylinder,			// collision cylinder
		const glm::vec3& feet,			// position of the feet in world space
		float eyes,						// distance from the feet to the eyes (camera view)
		float ankle						// distance from the feet to the ankles (can step over)
		);
	gaActor(flightRecorder::Entity* record);

	static void *create(void* record) {
		return new gaActor((flightRecorder::Entity*)record);
	}

	bool moveTo(time_t delta, glm::vec3& velocity);
	void rotate(const glm::vec3& direction);
	float height(void);
	float radius(void);
	inline float eyes(void) { return m_eyes; };
	inline float step(void) { return m_step; };
	inline const fwCylinder& cylinder(void) { return m_cylinder; };
	void jump(const glm::vec3& velocity);
	void parent(gaPlayer* parent) { m_parent = parent; };

	void dispatchMessage(gaMessage* message) override;

	int recordSize(void) override {
		return sizeof(flightRecorder::Actor);
	};														// size of one record

	uint32_t recordState(void* record) override;				// return a record of an actor state (for debug)
	void loadState(void* record) override;// reload an actor state from a record


	~gaActor();
};