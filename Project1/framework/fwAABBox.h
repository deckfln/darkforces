#pragma once

#include <glm/vec3.hpp>
#include "math/fwSphere.h"

class fwAABBox
{
public:
	float m_x, m_x1;
	float m_y, m_y1;
	float m_z, m_z1;

	fwAABBox();
	fwAABBox(float, float, float, float, float, float);
	fwAABBox(fwSphere& sphere);
	fwAABBox(glm::vec3& p1, glm::vec3& p2);
	bool inside(glm::vec3& position);
	bool intersect(fwAABBox& box);
	void extend(fwAABBox& box);
	void extend(glm::vec3& vertice);
	float surface(void);
	float volume(void);
	void reset(void);
	~fwAABBox();
};