#pragma once

#include "../dfObject.h"
#include "../dfComponent/dfCSprite/dfCSpriteAnimated.h"

namespace DarkForces {
	namespace Sprite {
		class WAX : public Object {
			DarkForces::Component::SpriteAnimated m_sprite;

		public:
			WAX(const std::string& model, const glm::vec3& p, float ambient, uint32_t objectid);
			WAX(const std::string& model, const glm::vec3& p, float ambient=1.0f);
		};
	}
}