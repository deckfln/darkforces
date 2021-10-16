#pragma once

#include <vector>
#include "../../gaEngine/gaBehaviorNode.h"

namespace DarkForces {
	namespace Behavior {
		class TrackPlayer : public GameEngine::BehaviorNode {
			std::vector<glm::vec3> m_navpoints;								// navigation for satnav
			uint32_t m_alarmID;

		public:
			TrackPlayer(const char* name);
			void init(void* data) override;									// init the node before running
			void execute(Action* r) override;								// let a parent take a decision with it's current running child
		};
	}
}