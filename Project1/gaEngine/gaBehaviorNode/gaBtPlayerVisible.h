#pragma once

#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class PlayerVisible : public BehaviorNode {
		public:
			PlayerVisible(const char* name);
			void execute(Action* r) override;						// let a parent take a decision with it's current running child

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			//debugger
			void debugGUInode(void) override;						// display the component in the debugger
		};
	}
}