#pragma once

/**
 * record the state of an AABBox
 */
#include "../../flightRecorder/classes.h"
#include "../../flightRecorder/Entity.h"

namespace flightRecorder {
	namespace DarkForces {
		struct Sector {
			struct flightRecorder::Entity entity;
			float ambient = 0;
		};
	}
}

extern void* frCreate_dfObject(void*);