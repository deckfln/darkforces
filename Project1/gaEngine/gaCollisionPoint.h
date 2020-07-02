#pragma once

#include "../framework/fwCollision.h"

class gaEntity;

class gaCollisionPoint : public fwCollisionPoint
{
	gaEntity* m_entity = nullptr;
public:
	gaCollisionPoint(fwCollisionLocation location, const glm::vec3& position, gaEntity *entity);
	gaEntity* entity(void) { return m_entity; };
	~gaCollisionPoint();
};