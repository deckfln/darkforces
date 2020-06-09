#pragma once
#include <glm/glm.hpp>

#include "../../glEngine/glBufferAttribute.h"

class fwBox3
{
	glm::vec3 m_min;
	glm::vec3 m_max;
	glm::vec3 m_center;

public:
	fwBox3();
	fwBox3 &setFromBufferAttribute(glBufferAttribute *attribute);
	const glm::vec3& center(void);
	~fwBox3();
};