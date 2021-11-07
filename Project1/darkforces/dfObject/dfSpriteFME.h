#pragma once

#include "../dfObject.h"
#include "../dfComponent/dfCSprite.h"

namespace DarkForces {
	namespace Sprite {
		class FME : public Object {
			DarkForces::Component::Sprite m_sprite;

		public:
			FME(const std::string& model, const glm::vec3& p, float ambient);
		};
	}
}