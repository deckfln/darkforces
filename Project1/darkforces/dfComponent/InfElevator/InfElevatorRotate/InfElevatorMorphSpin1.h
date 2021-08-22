#pragma once

#include "../InfElevatorRotate.h"

class dfSector;
class dfMesh;

namespace DarkForces {
	namespace Component {
		class InfElevatorMorphSpin1 : public InfElevatorRotate
		{
		public:
			InfElevatorMorphSpin1(dfSector* sector);
		};
	}
}
