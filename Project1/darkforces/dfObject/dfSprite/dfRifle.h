#pragma once

#include "../../dfObject.h"
#include "../../dfComponent/dfCSprite.h"

namespace DarkForces {
	namespace Sprite {
		class Rifle : public Object {
			DarkForces::Component::Sprite* m_sprite;

		public:
			Rifle(const glm::vec3& p, float ambient, uint32_t content);
		};
	}
}