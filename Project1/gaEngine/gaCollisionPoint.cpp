#include "gaCollisionPoint.h"

#include "gaEntity.h"

gaCollisionPoint::gaCollisionPoint(fwCollisionLocation location, const glm::vec3& position, gaEntity* entity) :
	fwCollisionPoint(location, position),
	m_entity(entity)
{
}

gaCollisionPoint::~gaCollisionPoint()
{
}
