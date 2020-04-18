#pragma once

#include <glm/vec3.hpp>
#include "math/fwSphere.h"

class fwAABBox
{
public:
	// create an impossible box, so 'extend' can work
	glm::vec3 m_p = glm::vec3(999999, 999999, 999999);
	glm::vec3 m_p1 = glm::vec3(-999999, -999999, -999999);

	fwAABBox();
	fwAABBox(float, float, float, float, float, float);
	fwAABBox(fwSphere& sphere);
	fwAABBox(glm::vec3& p1, glm::vec3& p2);
	fwAABBox(fwAABBox& source, glm::mat4& matrix);
	fwAABBox& multiplyBy(float v);
	bool inside(glm::vec3& position);
	bool intersect(fwAABBox& box);
	void extend(fwAABBox& box);
	void extend(glm::vec3& vertice);
	float surface(void);
	float volume(void);
	void reset(void);
	void translateFrom(fwAABBox& source, glm::vec3& translation);
	void rotateFrom(fwAABBox& source, glm::vec3& rotation);
	bool not_init(void);
	fwAABBox& copy(fwAABBox& source);
	~fwAABBox();
};