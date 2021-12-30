#pragma once

#pragma once

#include <vector>
#include "gaBVar.h"

namespace GameEngine {
	namespace Behavior {
		class SetVar : public Var {
		public:
			SetVar(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;

			void init(void*) override;

			// Behavior engine, GameEngine::BehaviorNode* used
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node
		};
	}
}