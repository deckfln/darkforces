#pragma once

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
	uint32_t m_triangle = -1;		// collision triangle
	void* m_source = nullptr;	// collision source
	Source m_class = Source::NONE;

	gaCollisionPoint(fwCollisionLocation location, const glm::vec3& position, uint32_t triangle);
	inline const uint32_t triangle(void) {return m_triangle; };
	~gaCollisionPoint();
};