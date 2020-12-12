#include "gaCollisionPoint.h"

#include "gaEntity.h"

gaCollisionPoint::gaCollisionPoint(fwCollisionLocation location, const glm::vec3& position, glm::vec3 const* triangle, int i) :
	fwCollisionPoint(location, position),
	m_triangleID(i)
{
	m_triangle[0] = triangle[0];
	m_triangle[1] = triangle[1];
	m_triangle[2] = triangle[2];
}

gaCollisionPoint::~gaCollisionPoint()
{
}
