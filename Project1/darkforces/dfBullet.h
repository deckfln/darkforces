#pragma once

#include <glm/vec3.hpp>

#include "../../framework/math/Segment.h"
#include "../../framework/fwTransforms.h"

#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaComponent/gaComponentMesh.h"
#include "../gaEngine/gaComponent/gaSound.h"

#include "weapons.h"

#include "../flightRecorder/frBullet.h"

class fwMesh;
class fwScene;
class gaMessage;

class dfBullet : public gaEntity
{
	glm::vec3 m_direction;				// direction of the bullet
	gaEntity* m_shooter;				// entity that shoot the bullet
	uint32_t m_damage;					// damage the bullet will do

	GameEngine::ComponentMesh m_componentMesh;
	GameEngine::Component::Sound m_sound;
	Framework::Segment m_segment;

	void tryToMove(void);
	void init(uint32_t damage, const glm::vec3& position, const glm::vec3& direction);

public:
	dfBullet(uint32_t damage, const glm::vec3& position, const glm::vec3& direction);
	dfBullet(flightRecorder::dfBullet *record);

	// getter/setter
	inline void shooter(gaEntity* entity) { m_shooter = entity; };

	static void* create(void* record) {
		return new dfBullet((flightRecorder::dfBullet*)record);
	}

	void dispatchMessage(gaMessage* message) override;

	// flight recorder data
	int recordSize(void) override {
		return sizeof(flightRecorder::dfBullet);
	};														// size of one record
	uint32_t recordState(void* record) override;				// return a record of an actor state (for debug)
	void loadState(void* record) override;// reload an actor state from a record

	~dfBullet();
};