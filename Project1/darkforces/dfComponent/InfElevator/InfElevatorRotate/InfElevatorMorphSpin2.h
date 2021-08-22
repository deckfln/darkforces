#pragma once

#include "../InfElevatorRotate.h"

class dfSector;
class dfMesh;

namespace DarkForces {
	namespace Component {
		class InfElevatorMorphSpin2 : public InfElevatorRotate
		{
		public:
			InfElevatorMorphSpin2(dfSector* sector);
		};
	}
}
