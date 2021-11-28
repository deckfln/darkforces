#pragma once

#include "../../gaEngine/gaBehaviorNode/gaBehaviorLoop.h"

#include <vector>

class gaEntity;

namespace DarkForces {
	namespace Behavior {
		class AttackPlayer : public GameEngine::Behavior::Loop
		{
			gaEntity* m_player=nullptr;										// player entity

		public:
			AttackPlayer(const char* name);
			void init(void* data) override;									// init the node before running

			// Behavior engine
			static BehaviorNode* create(const char* name);				// create a node

			// flightRecorder
			uint32_t recordState(void* record) override;
			uint32_t loadState(void* record) override;
		};
	}
}
