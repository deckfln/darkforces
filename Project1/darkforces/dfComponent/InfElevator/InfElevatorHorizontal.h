#pragma once

#include "../InfElevator.h"

class dfSector;
class dfMesh;

namespace DarkForces {
	namespace Component {
		class InfElevatorHorizontal : public InfElevator
		{
			void moveTo(float ambient) override;				// move the given position (depend on the elevator type)
			glm::vec3 m_move = glm::vec3(0);					// translation direction
		public:
			InfElevatorHorizontal(dfElevator::Type kind, dfSector* sector, bool smart=false);
			void angle(float angle);							// convert angle translation to a vector translation
			dfMesh* buildMesh(void) override;					// build the dfMesh of the elevator
			void dispatchMessage(gaMessage* message) override;	// deal with messages specifically for translation elevators
		};
	}
}
