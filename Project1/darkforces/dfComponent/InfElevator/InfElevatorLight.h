#pragma once

#include "../InfElevator.h"

class dfSector;

namespace DarkForces {
	namespace Component {
		class InfElevatorLight : public InfElevator 
		{
			void moveTo(float ambient) override;				// move the given position (depend on the elevator type)
		public:
			InfElevatorLight(const std::string& sector);
			InfElevatorLight(dfSector* sector);
		};
	}
}
