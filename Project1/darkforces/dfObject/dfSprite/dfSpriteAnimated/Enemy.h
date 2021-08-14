#pragma once

#include "../../dfSpriteAnimated.h"
#include "../../../../framework/math/fwCylinder.h"

namespace DarkForces {
	class Enemy : public dfSpriteAnimated
	{
		fwCylinder m_cylinder;						// player bounding cylinder

	public:
		Enemy(dfWAX* model, const glm::vec3& position, float ambient, uint32_t objectID);
		~Enemy();
	};
}