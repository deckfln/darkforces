#pragma once

#include "../../gaEngine/gaBehaviorNode.h"

class gaEntity;

namespace DarkForces {
	namespace Behavior {
		class Fire2Player : public GameEngine::BehaviorNode
		{
			gaEntity* m_player=nullptr;										// player entity
			glm::vec3 m_position;											// player position
			glm::vec3 m_direction;											// direction to the last player position
			uint32_t m_state = 0;
			uint32_t m_firingFrames = 0;
			uint32_t m_firingFrame = 0;

			bool locatePlayer(void);										// locate the player

		public:
			Fire2Player(const char* name);
			void init(void* data) override;									// init the node before running
			void dispatchMessage(gaMessage* message, Action* r) override;	// let a component deal with a situation
		};
	}
}
