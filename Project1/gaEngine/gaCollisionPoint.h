#pragma once

#include <glm/vec3.hpp>
#include "../framework/fwCollision.h"

class gaEntity;

class gaCollisionPoint : public fwCollisionPoint
{
public:
	enum class Source {
		NONE,
		ENTITY,
		SECTOR
	};
	void* m_source = nullptr;					// collision source
	int m_triangleID = 0;
	glm::vec3 m_triangle[3];					// collision triangle converted to world space
	Source m_class = Source::NONE;

	gaCollisionPoint(fwCollisionLocation location, const glm::vec3& position, glm::vec3 const* triangle, int index=0);
	~gaCollisionPoint();
};