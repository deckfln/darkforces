#pragma once

#include "../dfSprite.h"

namespace DarkForces {
	class IEnergy : public dfSprite {
	public:
		IEnergy(const glm::vec3& p, float ambient, uint32_t content);
	};
}