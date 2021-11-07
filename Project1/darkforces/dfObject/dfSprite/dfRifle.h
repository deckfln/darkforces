#pragma once

#include "../dfSprite.h"

namespace DarkForces {
	namespace Sprite {
		class Rifle : public dfSprite {
		public:
			Rifle(const glm::vec3& p, float ambient, uint32_t content);
		};
	}
}