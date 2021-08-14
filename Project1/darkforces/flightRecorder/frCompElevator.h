#pragma once

namespace flightRecorder {
	namespace DarkForces {
		struct CompElevator {
			uint32_t size;
			uint32_t id;
			uint32_t m_status;					// status of the elevator
			float m_tick;					// current timer
			float m_delay;					// time to run the elevator
			uint32_t m_currentStop;		// current stop for the running animation
			uint32_t m_nextStop;		// target altitude

			float m_current;				// current altitude of the part to move (floor or ceiling)
			float m_direction;				// direction and speed of the move
			float m_target;					// target altitude
		};
	}
}
