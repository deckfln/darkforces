#pragma once

#include <map>
#include "../../gaEngine/gaBehaviorNode/gaBSetVar.h"

namespace DarkForces {
	namespace Behavior {
		class SetVar : public GameEngine::Behavior::SetVar
		{
		public:
			SetVar(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void init(void*) override;

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node
		};
	}
}
