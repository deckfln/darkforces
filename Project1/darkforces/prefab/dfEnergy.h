#pragma once

#include "../dfObject/dfSpriteWAX.h"
#include "../dfComponent/dfComponentLogic.h"

namespace DarkForces {
	namespace Prefab
	{
		class Energy : public DarkForces::Sprite::WAX
		{
		public:
			Energy(const glm::vec3& p, uint32_t value);
		};
	}
}