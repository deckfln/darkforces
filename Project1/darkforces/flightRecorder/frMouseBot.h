#pragma once

#include <glm/vec3.hpp>

namespace flightRecorder {
	namespace DarkForces {
		struct MouseBot {
			uint32_t size;
			uint32_t id;

			glm::vec3 direction;
			float alpha;							// rotation angle for the direction
			int animation_time;					// time left to move in the direction
			bool active;							// is AI active
			uint32_t frame;						// reference to the last frame we received a message from
		};
	}
}
