#pragma once

#include <list>
#include "../../gaEngine/gaBehaviorNode.h"

class dfLogicTrigger;

namespace DarkForces {
	namespace Behavior {
		class OpenDoor : public GameEngine::BehaviorNode
		{
			std::list<dfLogicTrigger*> m_triggers;
			dfLogicTrigger* m_targetTrigger = nullptr;

		public:
			OpenDoor(const char* name);
			void init(void *) override;							// init the node before running
			GameEngine::BehaviorNode* nextNode(void) override;	// let a parent take a decision with it's current running child
		};
	}
}
