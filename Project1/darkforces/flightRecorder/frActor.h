#pragma once

#include "../../flightRecorder/Actor.h"

namespace flightRecorder {
	namespace DarkForces {
		struct Actor {
			struct flightRecorder::Actor actor;
			char currentSector[64];
		};
	}
}

extern void* frCreate_dfActor(void*);