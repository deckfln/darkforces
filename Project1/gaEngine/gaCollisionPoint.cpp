#include "gaCollisionPoint.h"

#include "gaEntity.h"

gaCollisionPoint::gaCollisionPoint(fwCollisionLocation location, const glm::vec3& position, glm::vec3 const* triangle, int i) :
	fwCollisionPoint(location, position),
	m_triangle(triangle),
	m_triangleID(i)
{
}

gaCollisionPoint::~gaCollisionPoint()
{
}
