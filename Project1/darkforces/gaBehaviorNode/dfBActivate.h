#pragma once

#pragma once

#include <vector>
#include "../../gaEngine/gaBehaviorNode.h"


namespace DarkForces {
	namespace Behavior {
		class Activate : public GameEngine::BehaviorNode
		{
			std::string m_variable;							// variable with the name of the trigger to activate

		public:
			Activate(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;

			void init(void *data) override;						// init the node before running

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node
		};
	}
}
