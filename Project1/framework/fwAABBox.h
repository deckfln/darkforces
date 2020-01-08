#pragma once

#include <glm/vec3.hpp>

class fwAABBox
{
public:
	float m_x, m_x1;
	float m_y, m_y1;
	float m_z, m_z1;

	fwAABBox();
	fwAABBox(float, float, float, float, float, float);
	bool inside(glm::vec3& position);
	bool intersect(fwAABBox& box);
	void extend(fwAABBox& box);
	float surface(void);
	float volume(void);
	~fwAABBox();
};