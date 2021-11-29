#pragma once

#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class Decorator : public BehaviorNode {
		public:	
			enum class Condition {
				STRAIGHT,		// return the status of the child
				INVERT,			// return the inverse of the child status
				FAILURE,		// always return failure
				SUCESS			// always return success
			};
			Decorator(const char* name);
			void execute(Action* r) override;						// let a parent take a decision with it's current running child

			inline void condition(Condition c) { m_condition = c; };

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			//debugger
			void debugGUInode(void) override;						// display the component in the debugger
		protected:
			Condition m_condition = Condition::STRAIGHT;
		};
	}
}