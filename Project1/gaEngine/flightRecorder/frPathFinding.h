#pragma once

#include <vector>
#include <glm/vec3.hpp>

namespace GameEngine {
	namespace flightRecorder {
		struct PathFinding {
			uint32_t size;
			uint32_t m_status;
			glm::vec3 m_destination;
			float m_speed = 0;
			uint32_t m_currentNavPoint;					// beware, backtrack as navpoints a	re in reverse order
			uint32_t c_navpoints;
			glm::vec3 m_navpoints[64];					// nav points for the move
			uint32_t c_previous;
			glm::vec3 m_previous[256];					// previous position
		};
	}
}