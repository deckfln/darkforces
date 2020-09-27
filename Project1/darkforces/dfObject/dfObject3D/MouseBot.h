#pragma once

#include <glm/vec3.hpp>

#include "../../framework/math/fwCylinder.h"

#include "../../dfModel/df3DO.h"
#include "../dfObject3D.h"
#include "../../dfComponent/dfComponentAI.h"

namespace DarkForces {
	class MouseBot : public dfObject3D 
	{
		fwCylinder m_cylinder;						// player bounding cylinder
		dfComponentAI m_ia;

	public:
		MouseBot(df3DO* threedo, const glm::vec3& position, float ambient, uint32_t objectID);
		~MouseBot();
	};
}