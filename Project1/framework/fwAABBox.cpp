#include "fwAABBox.h"
#include "fwAABBox.h"

#include <algorithm>    // std::min
#include "math/fwSphere.h"

fwAABBox::fwAABBox()
{
}

fwAABBox::fwAABBox(float x, float x1, float y, float y1, float z, float z1) :
	m_p(x, y, z),
	m_p1(x1, y1, z1)
{
}

fwAABBox::fwAABBox(fwSphere& sphere)
{
	glm::vec3 center = sphere.center();
	float radius = sphere.radius();

	m_p = glm::vec3(center.x - radius, center.y - radius, center.z - radius);
	m_p1 = glm::vec3(center.x + radius, center.y + radius, center.z + radius);
}

fwAABBox::fwAABBox(glm::vec3& p1, glm::vec3& p2)
{
	m_p = glm::min(p1, p2);
	m_p1 = glm::max(p1, p2);
}

fwAABBox::fwAABBox(fwAABBox& source, glm::mat4& matrix)
{
	m_p = glm::vec3(matrix * glm::vec4(source.m_p, 1.0));
	m_p1 = glm::vec3(matrix * glm::vec4(source.m_p1, 1.0));
}

fwAABBox& fwAABBox::multiplyBy(float v)
{
	m_p *= v;
	m_p1 *= v;

	return *this;
}

bool fwAABBox::inside(glm::vec3& position)
{
	return (position.x >= m_p.x && position.x <= m_p1.x &&
		position.y >= m_p.y && position.y <= m_p1.y &&
		position.z >= m_p.z && position.z <= m_p1.z);
}

/**
 * test if the 2 boxes intesect
 */
bool fwAABBox::intersect(fwAABBox& box)
{
	// using 6 splitting planes to rule out intersections.
	return (box.m_p1.x < m_p.x || box.m_p.x > m_p1.x ||
		box.m_p1.y < m_p.y || box.m_p.y > m_p1.y ||
		box.m_p1.z < m_p.z || box.m_p.z > m_p1.z) ? false : true;
}

void fwAABBox::extend(fwAABBox& box)
{
	if (m_p.x > box.m_p.x) m_p.x = box.m_p.x;
	if (m_p1.x < box.m_p1.x) m_p1.x = box.m_p1.x;
	if (m_p.y > box.m_p.y) m_p.y = box.m_p.y;
	if (m_p1.y < box.m_p1.y) m_p1.y = box.m_p1.y;
	if (m_p.z > box.m_p.z) m_p.z = box.m_p.z;
	if (m_p1.z < box.m_p1.z) m_p1.z = box.m_p1.z;
}

void fwAABBox::extend(glm::vec3& vertice)
{
	if (m_p.x > vertice.x) m_p.x = vertice.x;
	if (m_p1.x < vertice.x) m_p1.x = vertice.x;
	if (m_p.y > vertice.y) m_p.y = vertice.y;
	if (m_p1.y < vertice.y) m_p1.y = vertice.y;
	if (m_p.z > vertice.z) m_p.z = vertice.z;
	if (m_p1.z < vertice.z) m_p1.z = vertice.z;
}

float fwAABBox::surface(void)
{
	return (m_p1.x - m_p.x) * (m_p1.z - m_p.z);
}

float fwAABBox::volume(void)
{
	return (m_p1.x - m_p.x) * (m_p1.y - m_p.y) * (m_p1.z - m_p.z);
}

/**
 * Set all values back to NONE
 */
void fwAABBox::reset(void)
{
	m_p.x = m_p.y = m_p.z = 999999;
	m_p1.x = m_p1.y = m_p1.z = -99999;
}

fwAABBox::~fwAABBox()
{
}
