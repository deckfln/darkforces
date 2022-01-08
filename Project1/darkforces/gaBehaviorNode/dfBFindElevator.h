#pragma once

#include "../../gaEngine/gaBehaviorNode.h"
#include "../../gaEngine/gaVariable.h"

namespace DarkForces {
	namespace Behavior {
		class FindElevator : public GameEngine::BehaviorNode
		{
			GameEngine::Variable m_elevator;

		public:
			FindElevator(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void init(void*) override;

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node
		};
	}
}
