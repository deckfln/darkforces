#pragma once
/**
 * record the state of an object3D
 */
#include <time.h>
#include "../../flightRecorder/classes.h"
#include "frObject.h"

namespace flightRecorder {
	namespace DarkForces {
		struct Anim3D {
			uint32_t size;
			uint32_t id;

			glm::vec3 position_level;
			time_t lastFrame;			// timestamps of the last frame
			bool vue;
			time_t currentVueFrame;
		};
	}
}