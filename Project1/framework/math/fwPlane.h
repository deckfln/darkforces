#pragma once
#include <glm/glm.hpp>

class fwPlane
{
	glm::vec3 m_normal;
	float m_constant;

public:
	fwPlane();
	fwPlane(glm::vec3 _normal, float _contant);
	fwPlane &components(float x, float y, float z, float w);
	fwPlane &normalize(void);
	float distanceToPoint(const glm::vec3 &point);

	inline const glm::vec3& normal(void)  const { return m_normal; };
	inline const float constant(void)  const { return m_constant; };
	~fwPlane();
};