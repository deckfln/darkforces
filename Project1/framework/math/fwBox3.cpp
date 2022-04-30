#include "fwBox3.h"

fwBox3::fwBox3():
	m_min(INFINITY),
	m_max(-INFINITY)
{

}

fwBox3 &fwBox3::setFromBufferAttribute(glBufferAttribute *attribute)
{
	float minX = INFINITY;
	float minY = INFINITY;
	float minZ = INFINITY;

	float maxX = -INFINITY;
	float maxY = -INFINITY;
	float maxZ = -INFINITY;

	for (unsigned int i = 0; i < attribute->count(); ++i) {
		glm::vec3 *v = (glm::vec3 *)attribute->get_index(i);

		if (v->x < minX) minX = v->x;
		if (v->y < minY) minY = v->y;
		if (v->z < minZ) minZ = v->z;

		if (v->x > maxX) maxX = v->x;
		if (v->y > maxY) maxY = v->y;
		if (v->z > maxZ) maxZ = v->z;
	}

	m_min.x = minX;
	m_min.y = minY;
	m_min.z = minZ;

	m_max.x = maxX;
	m_max.y = maxY;
	m_max.z = maxZ;

	return *this;
}

const glm::vec3& fwBox3::center(void)
{
	m_center = (m_min + m_max) / 2.0f;

	return m_center;
}
