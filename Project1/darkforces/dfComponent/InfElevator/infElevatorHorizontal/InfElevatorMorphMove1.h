#pragma once

#include "../InfElevatorHorizontal.h"

class dfSector;
class dfMesh;

namespace DarkForces {
	namespace Component {
		class InfElevatorMorphMove1 : public InfElevatorHorizontal
		{
		public:
			InfElevatorMorphMove1(dfSector* sector);
		};
	}
}
