#pragma once

#include "../../gaEngine/gaBehaviorNode/gaBehaviorSequence.h"

class dfLogicTrigger;

namespace DarkForces {
	namespace Behavior {
		class OpenDoor : public GameEngine::Behavior::Sequence
		{
			void* m_collision = nullptr;

		public:
			OpenDoor(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void init(void *) override;							// init the node before running

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);	// create a node
		};
	}
}
