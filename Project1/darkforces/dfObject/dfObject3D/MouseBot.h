#pragma once

#include <glm/vec3.hpp>

#include "../../framework/math/fwCylinder.h"

#include "../df3DObject.h"
#include "../../dfComponent/AIMouseBot.h"

namespace DarkForces {
	namespace Anim {
		class MouseBot : public DarkForces::Anim::ThreeD
		{
			fwCylinder m_cylinder;						// player bounding cylinder
			AIMouseBot m_ia;

		public:
			MouseBot(const std::string& model, const glm::vec3& p, float ambient, uint32_t objectID);
		};
	}
}