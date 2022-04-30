#include "fwPlane.h"

fwPlane::fwPlane()
{

}

fwPlane::fwPlane(glm::vec3 _normal, float _constant):
	m_normal(_normal),
	m_constant(_constant)
{

}

fwPlane &fwPlane::components(float x, float y, float z, float w)
{
	m_normal.x = x;
	m_normal.y = y;
	m_normal.z = z;
	m_constant = w;

	return *this;
}

fwPlane &fwPlane::normalize(void) 
{
	// Note: will lead to a divide by zero if the plane is invalid.
	float l = glm::length(m_normal);
	float inverseNormalLength = 1.0 / l;
	m_normal *= inverseNormalLength;
	m_constant *= inverseNormalLength;
	return *this;
}

float fwPlane::distanceToPoint(const glm::vec3 &point)
{
	return glm::dot(m_normal, point) + m_constant;
}
