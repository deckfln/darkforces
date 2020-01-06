#include "fwAABBox.h"
#include "fwAABBox.h"

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

float fwAABBox::surface(void)
{
	return (m_x1 - m_x) * (m_z1 - m_z);
}

float fwAABBox::volume(void)
{
	return (m_x1 - m_x) * (m_y1 - m_y) * (m_z1 - m_z);
}

fwAABBox::~fwAABBox()
{
}
