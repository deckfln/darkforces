#pragma once

#include "Entity.h"

namespace flightRecorder {
	struct Actor {
		struct Entity entity;
		// fwCylinder m_cylinder;						// player bounding cylinder

		float speed;					// normal speed

		float ankle;								// maximum step the actor can walk up
		float eyes;								// position of the eyes (from the feet)
		float step;								// how up/down can the actor step over

		glm::mat3x3 physic;
		time_t animation_time;					// start of the physic driven movement
	};
}

extern void* frCreate_Actor(void*);