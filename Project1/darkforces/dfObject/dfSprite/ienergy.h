#pragma once

#include "../../dfObject/dfSpriteWAX.h"

namespace DarkForces {
	class IEnergy : public Sprite::WAX {
	public:
		IEnergy(const glm::vec3& p, float ambient, uint32_t content);
	};
}