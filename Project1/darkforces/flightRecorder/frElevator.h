#pragma once

#include "../../flightRecorder/Entity.h"

namespace flightRecorder {
	namespace DarkForces {
		struct Elevator {
			struct flightRecorder::Entity entity;
			int m_status;					// status of the elevator
			float m_tick;					// current timer
			float m_delay;					// time to run the elevator
			unsigned int m_currentStop;		// current stop for the running animation
			unsigned int m_nextStop;		// target altitude

			float m_current;				// current altitude of the part to move (floor or ceiling)
			float m_direction;				// direction and speed of the move
			float m_target;					// target altitude
		};
	}
}
