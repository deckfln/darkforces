#pragma once

#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class Sequence : public BehaviorNode {
		public:
			Sequence(const char* name);
			void init(void* data) override;
			void execute(Action* r) override;						// let a parent take a decision with it's current running child
			// debugger
			void debugGUInode(void) override;
		};
	}
}