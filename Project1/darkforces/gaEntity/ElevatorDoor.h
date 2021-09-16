#pragma once

#include "../../gaEngine/gaEntity.h"

class dfSector;

namespace DarkForces {
	namespace Entity {
		class ElevatorDoor : public gaEntity {
		public:
			ElevatorDoor(dfSector* sector);
			~ElevatorDoor();
		};
	}
}