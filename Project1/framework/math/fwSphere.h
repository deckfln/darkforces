#pragma once
#include <glm/glm.hpp>

class fwSphere
{
	glm::vec3 m_center;
	float m_radius = 0;
	int m_cache = -1;

public:
	fwSphere();
	int cache(int _cache) { m_cache = _cache;  return _cache; }
	int cache(void) { return m_cache; }
	glm::vec3 &center(void) { return m_center; }
	float radius(void) { return m_radius; }
	float radius(float _radius) { m_radius = _radius; return m_radius; }
	void applyMatrix4From(glm::mat4 &matrix, fwSphere *source);
	~fwSphere();
};