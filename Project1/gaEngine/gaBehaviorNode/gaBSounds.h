#pragma once

#include "gaBSetVar.h"

namespace GameEngine {
	namespace Behavior {
		class Sounds : public GameEngine::Behavior::SetVar
		{
		public:
			Sounds(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void init(void*) override;

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node
		};
	}
}
