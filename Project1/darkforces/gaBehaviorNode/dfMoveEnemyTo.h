#pragma once

#include "../../gaEngine/gaBehaviorNode.h"

namespace DarkForces {
	namespace Behavior {
		class MoveEnemyTo : public GameEngine::BehaviorNode
		{
			glm::vec3 m_destination;
		public:
			MoveEnemyTo(const char* name);
			BehaviorNode* dispatchMessage(gaMessage* message) override;	// let a component deal with a situation
			GameEngine::BehaviorNode* nextNode(void) override;	// let a parent take a decision with it's current running child
		};
	}
}
