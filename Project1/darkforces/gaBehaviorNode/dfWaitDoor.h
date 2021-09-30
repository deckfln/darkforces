#pragma once

#include "../../gaEngine/gaBehaviorNode.h"

namespace DarkForces {
	namespace Component {
		class InfElevator;
	}

	namespace Behavior {
		class WaitDoor : public GameEngine::BehaviorNode
		{
			Component::InfElevator* m_elevator;

		public:
			WaitDoor(const char* name);
			void dispatchMessage(gaMessage* message, Action* r) override;	// let a component deal with a situation
			void init(void* data) override;
		};
	}
}
