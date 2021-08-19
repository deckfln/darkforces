#include "gaCollisionPoint.h"

#include "gaEntity.h"

gaCollisionPoint::gaCollisionPoint(fwCollisionLocation location, const glm::vec3& position, glm::vec3 const* triangle, int i) :
	fwCollisionPoint(location, position),
	m_triangleID(i)
{
	if (triangle) {
		m_triangle[0] = triangle[0];
		m_triangle[1] = triangle[1];
		m_triangle[2] = triangle[2];
	}
}

gaCollisionPoint::gaCollisionPoint(fwCollisionLocation location, const glm::vec3& position, glm::vec3 const* triangle, int index,
	void* parent,
	gaCollisionPoint::Source parent_class):

	fwCollisionPoint(location, position),
	m_triangleID(index)

{
	if (triangle) {
		m_triangle[0] = triangle[0];
		m_triangle[1] = triangle[1];
		m_triangle[2] = triangle[2];
	}
	m_source = parent;
	m_class = parent_class;
}

gaCollisionPoint::~gaCollisionPoint()
{
}
