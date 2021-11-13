#pragma once

#include "../../gaEngine/gaComponent/gaSound.h"

#include "../dfObject.h"
#include "../dfComponent/dfCSprite/dfCSpriteAnimated.h"

class dfBulletExplode : public DarkForces::Object
{
	DarkForces::Component::SpriteAnimated* m_sprite=nullptr;
public:
	dfBulletExplode(const glm::vec3& position, float ambient);

	virtual void dispatchMessage(gaMessage* message);	// let an entity deal with a situation
	~dfBulletExplode();
};