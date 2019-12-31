#pragma once

#include <glm/vec3.hpp>

class fwAABBox
{
	float m_x, m_x1;
	float m_y, m_y1;
	float m_z, m_z1;
public:
	fwAABBox();
	fwAABBox(float, float, float, float, float, float);
	bool inside(glm::vec3& position);
	void extend(fwAABBox& box);
	~fwAABBox();
};