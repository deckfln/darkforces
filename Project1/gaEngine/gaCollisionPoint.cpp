#include "gaCollisionPoint.h"

#include "gaEntity.h"

gaCollisionPoint::gaCollisionPoint(fwCollisionLocation location, const glm::vec3& position, glm::vec3 const* triangle) :
	fwCollisionPoint(location, position),
	m_triangle(triangle)
{
}

gaCollisionPoint::~gaCollisionPoint()
{
}
