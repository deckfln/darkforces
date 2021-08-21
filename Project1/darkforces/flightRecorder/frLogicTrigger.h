#pragma once

#include "../../flightRecorder/Entity.h"

namespace flightRecorder {
	namespace DarkForces {
		struct LogicTrigger {
			struct flightRecorder::Entity entity;
			bool master = true;						// is the trigger operational ?
			bool actived = false;					// trigger was activated and shall not accept any new activation
		};
	}
}
