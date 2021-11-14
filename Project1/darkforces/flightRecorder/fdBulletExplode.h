#pragma once

#include <glm/vec3.hpp>
#include "frObject.h"

namespace flightRecorder {
	namespace DarkForces {
		struct BulletExplode {
			struct Object object;
			glm::vec3 lvlposition;
			float ambient;
		};
	}
}