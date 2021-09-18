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

			// flight recorder status
			uint32_t recordState(void* record) override;				// save the component state in a record
			uint32_t loadState(void* record) override;					// reload a component state from a record
		};
	}

	namespace FlightRecorder {
		struct MoveEnemyTo {
			GameEngine::FlightRecorder::BehaviorNode node;
			glm::vec3 destination;
			char wait_elevator[1];
		};
	}
}
