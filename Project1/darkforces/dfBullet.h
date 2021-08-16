#pragma once

#include <glm/vec3.hpp>

#include "../../framework/math/Segment.h"
#include "../../framework/fwTransforms.h"

#include "../gaEngine/gaEntity.h"
#include "../gaEngine/gaComponent/gaComponentMesh.h"

#include "../flightRecorder/frBullet.h"

class fwMesh;
class fwScene;
class gaMessage;

class dfBullet : public gaEntity
{
	glm::vec3 m_direction;
	GameEngine::ComponentMesh m_componentMesh;
	Framework::Segment m_segment;

	void tryToMove(void);

public:
	dfBullet(const glm::vec3& position, const glm::vec3& direction);
	dfBullet(flightRecorder::dfBullet *record);

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