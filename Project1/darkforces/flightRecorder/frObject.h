#pragma once

/**
 * record the state of an AABBox
 */
#include "../../flightRecorder/classes.h"
#include "../../flightRecorder/Entity.h"

namespace flightRecorder {
	namespace DarkForces {
		struct Object {
			struct flightRecorder::Entity entity;
			int is;
			uint32_t logics;
			uint32_t difficulty;
			float ambient;
			float radius;
			float height;
			char model[16];
			char sector[16];
			glm::vec3 position_level;
		};
	}
}
