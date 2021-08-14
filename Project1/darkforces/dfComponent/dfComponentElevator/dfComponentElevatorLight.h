#pragma once

#include "../dfComponentElevator.h"

class dfSector;

namespace DarkForces {
	namespace Component {
		class ElevatorLight : public DarkForces::Component::Elevator 
		{
			void moveTo(float ambient) override;				// move the given position (depend on the elevator type)
		public:
			ElevatorLight(const std::string& sector);
			ElevatorLight(dfSector* sector);
		};
	}
}
