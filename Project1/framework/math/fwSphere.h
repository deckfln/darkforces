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
	inline int cache(int _cache) { m_cache = _cache;  return _cache; }
	inline int cache(void) { return m_cache; }
	inline const glm::vec3 &center(void) { return m_center; }
	inline void center(const glm::vec3& center) { m_center = center; }
	inline float radius(void) { return m_radius; }
	inline float radius(float _radius) { m_radius = _radius; return m_radius; }
	inline void set(glm::vec3& center, float radius) { m_center = center; m_radius = radius; };
	void applyMatrix4From(const glm::mat4 &matrix, fwSphere *source);
	void applyMatrix4(glm::mat4& matrix);
};