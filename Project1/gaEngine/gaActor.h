#pragma once

#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

#include "../framework/fwAABBox.h"
#include "../framework/math/fwCylinder.h"

#include "gaEntity.h"
#include "Physics.h"
#include "gaComponent/gaListener.h"
#include "gaComponent/gaCImposter.h"
#include "gaComponent/gaActiveProbe.h"

#include "../flightRecorder/Actor.h"

const float c_jump = -c_gravity * 300.0f;

class dfLevel;
class gaPlayer;

class gaActor : public gaEntity
{
protected:
	fwCylinder m_cylinder;						// player bounding cylinder
	glm::vec3 m_direction;						// direction the actor looks at

	float m_speed = 0.0480f;					// normal speed

	float m_ankle = 0;							// maximum step the actor can walk up
	float m_eyes = 0;							// position of the eyes (from the feet)

	glm::mat3x3 m_physic = glm::mat3x3(0);
	time_t m_animation_time = 0;				// start of the physic driven movement

	gaPlayer* m_parent = nullptr;						// parent player
	GameEngine::Component::Listener m_listener;			// play the sound for real
	GameEngine::Component::Imposter m_imposter;			// physic imposter
	GameEngine::Component::ActiveProbe m_probe;			// forward probe

public:
	gaActor(
		uint32_t mclass,
		const std::string& name,		// name of the actor
		fwCylinder& cylinder,			// collision cylinder
		const glm::vec3& feet,			// position of the feet in world space
		float eyes,						// distance from the feet to the eyes (camera view)
		float ankle						// distance from the feet to the ankles (can step over)
	);
	gaActor(flightRecorder::Entity* record);

	static void* create(void* record) {
		return new gaActor((flightRecorder::Entity*)record);
	}

	bool moveTo(time_t delta, glm::vec3& velocity);
	void rotate(const glm::vec3& direction);
	float height(void);
	float radius(void);

	//getter/setter
	inline float eyes(void) { return m_eyes; };
	inline const fwCylinder& cylinder(void) { return m_cylinder; };
	inline const glm::vec3& direction(void) { return m_direction; };

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