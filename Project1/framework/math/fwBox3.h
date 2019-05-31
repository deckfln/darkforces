#pragma once
#include <glm/glm.hpp>

#include "../../glEngine/glBufferAttribute.h"

class fwBox3
{
	glm::vec3 m_min;
	glm::vec3 m_max;

public:
	fwBox3();
	fwBox3 &setFromBufferAttribute(glBufferAttribute *attribute);
	void get_center(glm::vec3 &center);
	~fwBox3();
};