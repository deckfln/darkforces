#pragma once

#include "../../gaEngine/gaBehaviorNode.h"

namespace DarkForces {
	namespace Behavior {
		class WaitIdle : public GameEngine::BehaviorNode
		{
			glm::vec3 m_original;							// still position of the enemy

		public:
			WaitIdle(const char* name);
			void activated(void) override;						// a node get re-activated after a child exit
			void dispatchMessage(gaMessage* message, Action* r) override;	// let a component deal with a situation
			void execute(Action* r) override;						// let a parent take a decision with it's current running child
		};
	}
}
