#pragma once

#include "../../gaEngine/gaBehaviorNode.h"

namespace DarkForces {
	namespace Behavior {
		class WaitDoor : public GameEngine::BehaviorNode
		{
		public:
			WaitDoor(const char* name);
			BehaviorNode* dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
			void init(void* data) override;
		};
	}
}
