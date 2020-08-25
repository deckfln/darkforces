#include "gaCollisionPoint.h"

#include "gaEntity.h"

gaCollisionPoint::gaCollisionPoint(fwCollisionLocation location, const glm::vec3& position, uint32_t triangle) :
	fwCollisionPoint(location, position),
	m_triangle(triangle)
{
}

gaCollisionPoint::~gaCollisionPoint()
{
}
