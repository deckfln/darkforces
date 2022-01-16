#pragma once

#include "../../gaEngine/flightRecorder/Actor.h"

namespace flightRecorder {
	namespace DarkForces {
		struct CompActor {
			struct GameEngine::Actor actor;
			uint32_t id;

			int32_t shield;
			int32_t maxShield;

			int32_t battery;
			int32_t life;

			uint32_t keys;

			char sector[16];
		};
	}
}

