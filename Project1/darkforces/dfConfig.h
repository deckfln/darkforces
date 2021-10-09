#pragma once

#include "../gaEngine/gaEntity.h"
#include "dfMessage.h"

namespace DarkForces {
	enum ClassID {
		Root = GameEngine::ClassID::Entity | (1<<2),
		Sector = Root | (1<<5),
		Bullet = Root | (1<<6),
		Elevator = Root | (1<<7),
		Trigger = Root | (1<<8),
		Object = Root | (1<<9)
	};
}
