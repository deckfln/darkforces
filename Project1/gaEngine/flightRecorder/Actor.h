#pragma once

#include <glm/vec3.hpp>

namespace flightRecorder {
	namespace GameEngine {
		struct Actor {
			uint32_t size;
			glm::vec3 direction;
		};
	}
}
