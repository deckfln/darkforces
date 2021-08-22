#pragma once

#include "../InfElevator.h"

class dfSector;
class dfMesh;

namespace DarkForces {
	namespace Component {
		class InfElevatorTranslate : public InfElevator
		{
			void moveTo(float ambient) override;				// move the given position (depend on the elevator type)
		public:
			InfElevatorTranslate(dfElevator::Type kind, dfSector* sector, bool smart=false);
			dfMesh* buildMesh(void) override;					// build the dfMesh of the elevator
			void dispatchMessage(gaMessage* message) override;	// deal with messages specifically for translation elevators
		};
	}
}
