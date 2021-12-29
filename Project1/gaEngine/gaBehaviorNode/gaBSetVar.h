#pragma once

#pragma once

#include <vector>
#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class SetVar : public BehaviorNode {
			std::string m_variable;						// list of angles to turn to
			bool m_value;							// current one
		public:
			SetVar(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;

			void init(void*) override;

			// Behavior engine, GameEngine::BehaviorNode* used
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			// debugger
			void debugGUInode(void) override;							// display the component in the debugger
		};
	}
}