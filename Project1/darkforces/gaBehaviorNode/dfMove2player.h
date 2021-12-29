#pragma once

#include <vector>
#include "../../gaEngine/gaBehaviorNode/gaBehaviorDecorator.h"

class gaEntity;

namespace DarkForces {
	namespace Behavior {
		class Move2Player : public GameEngine::Behavior::Decorator
		{
			glm::vec3 m_target;												// move

		public:
			Move2Player(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void init(void* data) override;									// init the node before running

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			//debugger
			void debugGUInode(void) override;							// display the component in the debugger
		};
	}
}
