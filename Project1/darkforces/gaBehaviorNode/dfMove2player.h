#pragma once

#include <vector>
#include "../../gaEngine/gaBehaviorNode/gaBehaviorDecorator.h"

class gaEntity;

namespace DarkForces {
	namespace Behavior {
		class Move2Player : public GameEngine::Behavior::Decorator
		{
			gaEntity* m_player=nullptr;										// player entity
			glm::vec3 m_lastKnwonPosition;									// player position
			glm::vec3 m_direction;											// direction to the last player position
			uint32_t m_alarmID=0;											// alarm programmed to cancel the move node

			std::vector<glm::vec3> m_navpoints;								// navigation for satnav

		protected:
			void onChildExit(Status status) override;

		public:
			Move2Player(const char* name);
			void init(void* data) override;									// init the node before running
		};
	}
}
