#pragma once

#include "dfSpriteAnimated.h"
#include "../../gaEngine/gaComponent/gaSound.h"

class dfBulletExplode : public dfSpriteAnimated
{
	GameEngine::Component::Sound m_sound;
public:
	dfBulletExplode(const glm::vec3& position, float ambient);
	dfBulletExplode(flightRecorder::DarkForces::SpriteAnimated*);

	static void* create(void* record) {
		return new dfBulletExplode((flightRecorder::DarkForces::SpriteAnimated*)record);
	}

	virtual void dispatchMessage(gaMessage* message);	// let an entity deal with a situation
	~dfBulletExplode();
};