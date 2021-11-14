#pragma once

#include <map>

#include "dfVOC.h"

namespace DarkForces {
	enum Sounds {
		PLAYER_HIT_BY_STORM_COMMANDO_OFFICER=1024,
		PLAYER_NEARLY_HIT,
		WALL_HIT_LASER,
		STORM_COMMANDO_OFFICER_HIT_LASER,
		STORM_COMMANDO_OFFICER_DIE,
		MOUSEBOT_EEK,
		MOUSEBOT_DIE,
	};

	dfVOC* loadSound(uint32_t soundID);
}