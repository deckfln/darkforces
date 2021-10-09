#pragma once

#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class Loop : public BehaviorNode {
		public:
			Loop(const char* name);
			void execute(Action* r) override;						// let a parent take a decision with it's current running child
		};
	}
}