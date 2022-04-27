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
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void execute(Action* r) override;						// let a parent take a decision with it's current running child

			inline void condition(Condition c) { m_condition = c; };

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

#ifdef _DEBUG
			//debugger
			void debugGUInode(GameEngine::Component::BehaviorTree* tree) override;	// display the component in the debugger
#endif
		protected:
			Condition m_condition = Condition::STRAIGHT;
		};
	}
}