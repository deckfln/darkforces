#pragma once

#include "../InfElevatorTranslate.h"

class dfSector;
class dfMesh;

namespace DarkForces {
	namespace Component {
		class InfElevatorMoveCeiling : public InfElevatorTranslate
		{
		public:
			InfElevatorMoveCeiling(dfSector* sector);
			dfMesh* buildMesh(void) override;					// build the dfMesh of the elevator
			void relocateMesh(dfMesh* mesh) override;					// move the mesh vertices into a 0,0,0 position
		};
	}
}
