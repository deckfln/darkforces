#pragma once

#include "../framework/fwCollision.h"

class gaEntity;

class gaCollisionPoint : public fwCollisionPoint
{
	uint32_t m_triangle=-1;	// collision triangle

public:
	gaCollisionPoint(fwCollisionLocation location, const glm::vec3& position, uint32_t triangle);
	inline const uint32_t triangle(void) {return m_triangle; };
	~gaCollisionPoint();
};