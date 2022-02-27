#pragma once

#include "../../gaEngine/gaBehaviorNode.h"


namespace DarkForces {
	namespace Behavior {
		class State : public GameEngine::BehaviorNode
		{
			uint32_t m_state = 0;

		public:
			State(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void execute(Action* r) override;

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node
		};
	}
}
