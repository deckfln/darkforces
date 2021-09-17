#pragma once

#pragma once

#include <list>
#include "../../gaEngine/gaBehaviorNode.h"


namespace DarkForces {
	namespace Component {
		class Trigger;
	};

	class Compo;
	namespace Behavior {
		class GotoTrigger : public GameEngine::BehaviorNode
		{
			std::list<gaEntity*> m_triggers;
			gaEntity* m_targetTrigger = nullptr;
			void activate_trigger(void);
			void goto_next_trigger(void);

		public:
			GotoTrigger(const char* name);
			void init(void *) override;							// init the node before running
			GameEngine::BehaviorNode* nextNode(void) override;	// let a parent take a decision with it's current running child
		};
	}
}
