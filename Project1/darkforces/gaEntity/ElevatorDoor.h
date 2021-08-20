#pragma once

#include "../../gaEngine/gaEntity.h"

class dfSector;
class dfLogicTrigger;

namespace DarkForces {
	namespace Entity {
		class ElevatorDoor : public gaEntity {
			dfLogicTrigger* m_trigger = nullptr;

		public:
			ElevatorDoor(dfSector* sector);
			inline dfLogicTrigger* trigger(void) { return m_trigger; };
			~ElevatorDoor();
		};
	}
}