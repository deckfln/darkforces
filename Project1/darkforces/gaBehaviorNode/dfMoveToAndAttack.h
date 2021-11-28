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
			void execute(Action* r) override;								// let a parent take a decision with it's current running child

			// Behavior engine
			static BehaviorNode* create(const char* name);				// create a node
		};
	}
}
