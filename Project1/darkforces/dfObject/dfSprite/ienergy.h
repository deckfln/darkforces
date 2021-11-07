#pragma once

#include "../../dfObject.h"
#include "../../dfComponent/dfCSprite.h"

namespace DarkForces {
	class IEnergy : public Object {
		DarkForces::Component::Sprite* m_sprite;

	public:
		IEnergy(const glm::vec3& p, float ambient, uint32_t content);
	};
}