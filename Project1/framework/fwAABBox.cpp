#include "fwAABBox.h"
#include "fwAABBox.h"

#include "math/fwSphere.h"

fwAABBox::fwAABBox()
{
	// create an impossible box, so 'extend' can work
	m_x = m_y = m_z = 999999;
	m_x1 = m_y1 = m_z1 = -99999;
}

fwAABBox::fwAABBox(float x, float x1, float y, float y1, float z, float z1) :
	m_x(x),
	m_x1(x1),
	m_y(y),
	m_y1(y1),
	m_z(z),
	m_z1(z1)
{
}

fwAABBox::fwAABBox(fwSphere& sphere)
{
	glm::vec3 center = sphere.center();
	float radius = sphere.radius();

	m_x = center.x - radius;
	m_x1 = center.x + radius;
	m_y = center.y - radius;
	m_y1 = center.y + radius;
	m_z = center.z - radius;
	m_z1 = center.z + radius;
}

bool fwAABBox::inside(glm::vec3& position)
{
	return (position.x >= m_x && position.x <= m_x1 &&
		position.y >= m_y && position.y <= m_y1 &&
		position.z >= m_z && position.z <= m_z1);
}

/**
 * test if the 2 boxes intesect
 */
bool fwAABBox::intersect(fwAABBox& box)
{
	// using 6 splitting planes to rule out intersections.
	return (box.m_x1 < m_x || box.m_x > m_x1 ||
		box.m_y1 < m_y || box.m_y > m_y1 ||
		box.m_z1 < m_z || box.m_z > m_z1) ? false : true;
}

void fwAABBox::extend(fwAABBox& box)
{
	if (m_x > box.m_x) m_x = box.m_x;
	if (m_x1 < box.m_x1) m_x1 = box.m_x1;
	if (m_y > box.m_y) m_y = box.m_y;
	if (m_y1 < box.m_y1) m_y1 = box.m_y1;
	if (m_z > box.m_z) m_z = box.m_z;
	if (m_z1 < box.m_z1) m_z1 = box.m_z1;
}

void fwAABBox::extend(glm::vec3& vertice)
{
	if (m_x > vertice.x) m_x = vertice.x;
	if (m_x1 < vertice.x) m_x1 = vertice.x;
	if (m_y > vertice.y) m_y = vertice.y;
	if (m_y1 < vertice.y) m_y1 = vertice.y;
	if (m_z > vertice.z) m_z = vertice.z;
	if (m_z1 < vertice.z) m_z1 = vertice.z;
}

float fwAABBox::surface(void)
{
	return (m_x1 - m_x) * (m_z1 - m_z);
}

float fwAABBox::volume(void)
{
	return (m_x1 - m_x) * (m_y1 - m_y) * (m_z1 - m_z);
}

/**
 * Set all values back to NONE
 */
void fwAABBox::reset(void)
{
	m_x = m_y = m_z = 999999;
	m_x1 = m_y1 = m_z1 = -99999;
}

fwAABBox::~fwAABBox()
{
}
