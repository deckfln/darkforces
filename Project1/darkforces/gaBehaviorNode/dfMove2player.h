#pragma once

#include <vector>
#include "../../gaEngine/gaBehaviorNode/gaBSetVar.h"

class gaEntity;

namespace DarkForces {
	namespace Behavior {
		class Move2Player : public GameEngine::Behavior::SetVar
		{
		public:
			Move2Player(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void init(void* data) override;									// init the node before running

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node
		};
	}
}
