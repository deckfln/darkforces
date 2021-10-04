#pragma once

#include "../../gaEngine/gaBehaviorNode.h"

class dfLogicTrigger;

namespace DarkForces {
	namespace Behavior {
		class OpenDoor : public GameEngine::BehaviorNode
		{
			void* m_collision = nullptr;

		public:
			OpenDoor(const char* name);
			void init(void *) override;							// init the node before running
			void execute(Action* r) override;					// let a parent take a decision with it's current running child
		};
	}
}
