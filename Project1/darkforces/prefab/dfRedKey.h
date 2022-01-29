#pragma once

#include "../dfObject/dfSpriteFME.h"
#include "../dfComponent/dfComponentLogic.h"

namespace DarkForces {
	namespace Prefab
	{
		class RedKey : public DarkForces::Sprite::FME
		{
		public:
			RedKey(const glm::vec3& p);
		};
	}
}