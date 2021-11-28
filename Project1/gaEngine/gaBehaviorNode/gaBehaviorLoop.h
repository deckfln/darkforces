#pragma once

#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class Loop : public BehaviorNode {
		public:
			enum class Condition {
				UNTIL_ALL_FAIL,
				UNTIL_ONE_FAIL
			};
				
			Loop(const char* name);
			void execute(Action* r) override;						// let a parent take a decision with it's current running child

			// Behavior engine
			static BehaviorNode* create(const char* name);					// create a node

			//debugger
			void debugGUInode(void) override;						// display the component in the debugger
		protected:
			Condition m_condition = Condition::UNTIL_ALL_FAIL;
		};
	}
}