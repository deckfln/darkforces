#pragma once

#pragma once

#include <vector>
#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class CheckVar : public BehaviorNode {
			std::string m_name;						// list of angles to turn to
			bool m_value;							// current one
		public:
			CheckVar(const char* name);

			void init(void*) override;

			// Behavior engine, GameEngine::BehaviorNode* used
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			// debugger
			void debugGUInode(void) override;							// display the component in the debugger
		};
	}
}