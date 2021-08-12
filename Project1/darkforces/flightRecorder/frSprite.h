#pragma once

#include "../../flightRecorder/classes.h"
#include "frObject.h"

namespace flightRecorder {
	namespace DarkForces {
		struct Sprite {
			struct flightRecorder::DarkForces::dfObject object;
		};
	}
}

extern void* frCreate_dfSpriteAnimated(void*);