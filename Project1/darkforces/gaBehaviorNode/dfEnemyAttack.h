#pragma once

#include "../../gaEngine/gaBehaviorNode.h"

class gaEntity;

namespace DarkForces {
	namespace Behavior {
		class EnemyAttack : public GameEngine::BehaviorNode
		{
			gaEntity* m_player=nullptr;										// player entity
			glm::vec3 m_position;											// player position

		public:
			EnemyAttack(const char* name);
			void init(void* data) override;									// init the node before running
			void execute(Action* r) override;								// let a parent take a decision with it's current running child

			void dispatchMessage(gaMessage* message, Action* r) override;	// let a component deal with a situation
		};
	}
}
