#pragma once

#include "../../gaEngine/gaBehaviorNode.h"

namespace DarkForces {
	namespace Behavior {
		class FindElevator : public GameEngine::BehaviorNode
		{
		public:
			FindElevator(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void init(void*) override;

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node
		};
	}
}
