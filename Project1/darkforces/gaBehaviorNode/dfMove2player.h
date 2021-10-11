#pragma once

#include <vector>
#include "../../gaEngine/gaBehaviorNode.h"

class gaEntity;

namespace DarkForces {
	namespace Behavior {
		class Move2Player : public GameEngine::BehaviorNode
		{
			gaEntity* m_player=nullptr;										// player entity
			glm::vec3 m_lastKnwonPosition;									// player position
			glm::vec3 m_direction;											// direction to the last player position
			uint32_t m_alarmID;												// alarm programmed to cancel the move node

			std::vector<glm::vec3> m_navpoints;								// navigation for satnav

			bool locatePlayer(void);										// locate the player

		public:
			Move2Player(const char* name);
			void init(void* data) override;									// init the node before running
			void execute(Action* r) override;								// let a parent take a decision with it's current running child
		};
	}
}
