#pragma once

#include "../gaEngine/gaEntity.h"
#include "dfMessage.h"

namespace DarkForces {
	enum ClassID {
		_Root = GameEngine::ClassID::Entity | (1<<2),
		_Sector = _Root | (1<<5),
		_Bullet = _Root | (1<<6),
		_Elevator = _Root | (1<<7),
		_Trigger = _Root | (1<<8),
		_Object = _Root | (1<<9),
		_Goals = _Root | (1<<10),
		_HUD = _Root | (1 << 11),
		_PDA = _Root | (1 << 12)
	};
}
