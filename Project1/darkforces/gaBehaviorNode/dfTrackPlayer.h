#pragma once

#include <vector>
#include "../../gaEngine/gaBehaviorNode/gaBehaviorDecorator.h"

namespace DarkForces {
	namespace Behavior {
		class TrackPlayer : public GameEngine::Behavior::Decorator {
			std::vector<glm::vec3> m_navpoints;								// navigation for satnav
			uint32_t m_alarmID;

		protected:
			void onChildExit(uint32_t child, Status status) override;

		public:
			TrackPlayer(const char* name);
			void init(void* data) override;									// init the node before running

			// Behavior engine
			static BehaviorNode* create(const char* name);				// create a node
		};
	}
}