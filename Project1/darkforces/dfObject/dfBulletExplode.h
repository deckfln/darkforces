#pragma once

#include "../../gaEngine/gaComponent/gaSound.h"

#include "../dfObject.h"
#include "../dfComponent/dfCSprite/dfCSpriteAnimated.h"

#include "../flightRecorder/fdBulletExplode.h"

class dfBulletExplode : public DarkForces::Object
{
	DarkForces::Component::SpriteAnimated* m_sprite=nullptr;
	void init(const glm::vec3& position, float ambient);
public:
	dfBulletExplode(const glm::vec3& position, float ambient);
	dfBulletExplode(flightRecorder::DarkForces::BulletExplode* r);

	virtual void dispatchMessage(gaMessage* message);	// let an entity deal with a situation

	// flight recorder data
	static void* create(void* record) {
		return new dfBulletExplode((flightRecorder::DarkForces::BulletExplode*)record);
	}
	int recordSize(void) override {
		return sizeof(flightRecorder::DarkForces::BulletExplode);
	};													// size of one record
	uint32_t recordState(void* record) override;		// return a record of an actor state (for debug)
	void loadState(void* record) override;				// reload an actor state from a record

	~dfBulletExplode();
};