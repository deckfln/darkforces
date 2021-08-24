#pragma once

#include "../InfElevator.h"

class dfSector;
class dfMesh;

namespace DarkForces {
	namespace Component {
		class InfElevatorRotate : public InfElevator
		{
			void moveTo(float ambient) override;				// move the given position (depend on the elevator type)
		public:
			InfElevatorRotate(DarkForces::Component::InfElevator::Type kind, dfSector* sector, bool smart=false);
			dfMesh* buildMesh(void) override;					// build the dfMesh of the elevator
			void relocateMesh(dfMesh* mesh) override;
			void dispatchMessage(gaMessage* message) override;	// deal with messages specifically for translation elevators
		};
	}
}
