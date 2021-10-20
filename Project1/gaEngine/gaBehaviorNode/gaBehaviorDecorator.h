#pragma once

#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class Decorator : public BehaviorNode {
		public:	
			Decorator(const char* name);
			void execute(Action* r) override;						// let a parent take a decision with it's current running child

			//debugger
			void debugGUInode(void) override;						// display the component in the debugger
		protected:
			inline virtual void onChildExit(Status status) {};
		};
	}
}