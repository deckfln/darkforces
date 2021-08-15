#pragma once

/**
 * record the state of an AABBox
 */
#include "../../flightRecorder/classes.h"
#include "../../flightRecorder/Entity.h"

namespace flightRecorder {
	namespace DarkForces {
		struct dfObject {
			struct flightRecorder::Entity entity;
			glm::vec3 position_level;
		};
	}
}
