#pragma once
#include <glm/glm.hpp>

class fwCylinder;

class fwSphere
{
	glm::vec3 m_center = glm::vec3(0);
	float m_radius = 0;
	int m_cache = -1;

public:
	fwSphere();
	fwSphere(float radius);
	fwSphere(glm::vec3 &center, float radius);
	fwSphere(fwCylinder& cyl, bool inside);
	int cache(int _cache) { m_cache = _cache;  return _cache; }
	int cache(void) { return m_cache; }
	const glm::vec3 &center(void) { return m_center; }
	void center(const glm::vec3& center) { m_center = center; }
	float radius(void) { return m_radius; }
	float radius(float _radius) { m_radius = _radius; return m_radius; }
	void set(glm::vec3& center, float radius) { m_center = center; m_radius = radius; };
	void applyMatrix4From(const glm::mat4 &matrix, fwSphere *source);
	void applyMatrix4(glm::mat4& matrix);
};