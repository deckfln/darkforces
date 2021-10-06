#pragma once

#include "../../gaEngine/gaBehaviorNode.h"

class gaEntity;

namespace DarkForces {
	namespace Behavior {
		class EnemyAttack : public GameEngine::BehaviorNode
		{
			gaEntity* m_player=nullptr;										// player entity
			glm::vec3 m_position;											// player position
			glm::vec3 m_direction;											// direction to the last player position
			float m_steps = 0;												// number of steps since last check

			void locatePlayer(void);										// locate the player
			void triggerMove(void);											// trigger a move toward the player
			void onMove(gaMessage* message, Action* r);						// manage move actions

		public:
			EnemyAttack(const char* name);
			void init(void* data) override;									// init the node before running

			void dispatchMessage(gaMessage* message, Action* r) override;	// let a component deal with a situation
		};
	}
}
