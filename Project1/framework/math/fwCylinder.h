#pragma once

#include <glm/vec3.hpp>

class fwCylinder {
	glm::vec3 m_position = glm::vec3(0);	// center of the botton part
	float m_radius = 0;
	float m_height = 0;
public:
	fwCylinder();
	fwCylinder(const glm::vec3& position, float radius, float height);
	fwCylinder(const fwCylinder& source, const glm::vec3 translation);
	fwCylinder& copy(const fwCylinder& source);

	glm::vec3& position(void) { return m_position; };
	void position(glm::vec3& p) { m_position = p; };
	float radius(void) { return m_radius; };
	void radius(float r) { m_radius = r; };
	float height(void) { return m_height; };
	void height(float h) { m_height = h; };
};