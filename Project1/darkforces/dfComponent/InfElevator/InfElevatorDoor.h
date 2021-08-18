#pragma once

#include "../InfElevator.h"

class dfSector;

namespace DarkForces {
	namespace Component {
		class InfElevatorDoor : public InfElevator
		{
			void moveTo(float ambient) override;				// move the given position (depend on the elevator type)
		public:
			InfElevatorDoor(const std::string& sector);
			InfElevatorDoor(dfSector* sector);
		};
	}
}
