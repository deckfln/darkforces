#pragma once

#include "../../dfObject/dfSpriteWAX.h"

namespace DarkForces {
	namespace Sprite {
		class Rifle : public DarkForces::Sprite::WAX {
		public:
			Rifle(const glm::vec3& p, float ambient, uint32_t content);
		};
	}
}