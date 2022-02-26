#pragma once

#pragma once

#include <vector>
#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class Alarm : public BehaviorNode {
			int m_minDelay = 0;
			int m_maxDelay = 0;
			uint32_t m_timer = 0;
			uint32_t m_message = 0;

		public:
			Alarm(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;

			void init(void*) override;

			// Behavior engine, GameEngine::BehaviorNode* used
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			// debugger
			void debugGUInode(GameEngine::Component::BehaviorTree* tree) override;	// display the component in the debugger
		};
	}
}