#pragma once

#include "../InfElevator.h"

class dfSector;

namespace DarkForces {
	namespace Component {
		class InfElevatorLight : public InfElevator 
		{
			bool m_animate = false;
			void moveTo(float ambient) override;				// move the given position (depend on the elevator type)
		public:
			InfElevatorLight(dfSector* sector);

			void dispatchMessage(gaMessage* message);           // let an entity deal with a situation
		};
	}
}
