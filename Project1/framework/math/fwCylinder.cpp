#include "fwCylinder.h"

fwCylinder::fwCylinder()
{
}

fwCylinder::fwCylinder(const glm::vec3& position, float radius, float height):
	m_position(position),
	m_radius(radius),
	m_height(height)
{
}

fwCylinder::fwCylinder(const fwCylinder& source, const glm::vec3 translation):
	m_position(source.m_position + translation),
	m_radius(source.m_radius),
	m_height(source.m_height)
{
}

fwCylinder& fwCylinder::copy(const fwCylinder& source, float ratio)
{
	m_position = source.m_position;
	m_height = source.m_height * ratio;
	m_radius = source.m_radius * ratio;

	return *this;
}

/**
 * Distance between 2 cylinders
 */
glm::vec3 fwCylinder::to(const fwCylinder& to)
{
	return m_position - to.m_position;
}
