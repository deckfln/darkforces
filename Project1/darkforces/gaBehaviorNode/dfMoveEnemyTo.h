#pragma once

#include "../../gaEngine/gaBehaviorNode.h"

namespace DarkForces {
	namespace Behavior {
		class MoveEnemyTo : public GameEngine::BehaviorNode
		{
			glm::vec3 m_destination;
		public:
			MoveEnemyTo(const char* name);
			void init(void* data) override;
			void execute(Action* r) override;							// let a parent take a decision with it's current running child

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

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
