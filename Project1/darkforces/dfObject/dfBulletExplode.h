#pragma once

#include "dfSpriteAnimated.h"

class dfBulletExplode : public dfSpriteAnimated
{
public:
	dfBulletExplode(const glm::vec3& position, float ambient);
	virtual void dispatchMessage(gaMessage* message);	// let an entity deal with a situation
	~dfBulletExplode();
};