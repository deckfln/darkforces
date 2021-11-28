#pragma once

#pragma once

#include <vector>
#include "../../gaEngine/gaBehaviorNode.h"


namespace DarkForces {
	namespace Component {
		class Trigger;
	};

	class Compo;
	namespace Behavior {
		class GotoTrigger : public GameEngine::BehaviorNode
		{
			std::vector<gaEntity*> m_triggers;
			int32_t m_next;
			gaEntity* m_targetTrigger = nullptr;
			void activate_trigger(Action* r);
			void goto_next_trigger(Action *r);

		public:
			GotoTrigger(const char* name);
			void init(void *data) override;						// init the node before running
			void execute(Action* r) override;					// let a parent take a decision with it's current running child

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element);// create a node

			// flight recorder status
			uint32_t recordState(void* record) override;		// save the component state in a record
			uint32_t loadState(void* record) override;			// reload a component state from a record
		};
	}

	namespace FlightRecorder {
		struct GotoTrigger {
			GameEngine::FlightRecorder::BehaviorNode node;
			uint32_t nbTriggers;
			uint32_t target;
			uint32_t next;
			char triggers[1];
		};
	}

}
