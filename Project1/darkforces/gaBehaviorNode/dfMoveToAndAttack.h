#pragma once

#include <vector>
#include "../../gaEngine/gaBehaviorNode/gaBehaviorLoop.h"

class gaEntity;

namespace DarkForces {
	namespace Behavior {
		class MoveToAndAttack : public GameEngine::Behavior::Loop
		{
		public:
			MoveToAndAttack(const char* name);
			void init(void* data) override;									// init the node before running
			void execute(Action* r) override;								// let a parent take a decision with it's current running child
		};
	}
}
