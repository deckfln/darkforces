#pragma once

#include <vector>
#include <glm/vec3.hpp>

namespace GameEngine {
	namespace flightRecorder {
		struct BehaviorTree {
			uint32_t size;
			uint32_t nbNodes;
			uint32_t m_current;
			char nodes[1024 * 5];
		};
	}
}