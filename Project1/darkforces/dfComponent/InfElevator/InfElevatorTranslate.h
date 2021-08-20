#pragma once

#include "../InfElevator.h"

class dfSector;

namespace DarkForces {
	namespace Component {
		class InfElevatorTranslate : public InfElevator
		{
			void moveTo(float ambient) override;				// move the given position (depend on the elevator type)
		public:
			InfElevatorTranslate(dfElevator::Type kind, dfSector* sector);
		};
	}
}
