#pragma once

namespace flightRecorder {
	namespace DarkForces {
		struct CompActor {
			uint32_t size;
			uint32_t id;

			int32_t shield;
			int32_t maxShield;

			int32_t energy;
			int32_t maxEnergy;

			int32_t battery;
			int32_t life;

			uint32_t keys;

			char sector[16];
		};
	}
}

