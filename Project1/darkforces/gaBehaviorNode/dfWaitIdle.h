#pragma once

#include <map>
#include "../../gaEngine/gaBehaviorNode.h"

namespace DarkForces {
	namespace Behavior {
		class WaitIdle : public GameEngine::BehaviorNode
		{
			glm::vec3 m_original;							// still position of the enemy

		public:
			WaitIdle(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void activated(void) override;						// a node get re-activated after a child exit
			//void init(void*) override;
			void execute(Action* r) override;
			void dispatchMessage(gaMessage* message, Action* r) override;	// let a component deal with a situation

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node
		};
	}
}
