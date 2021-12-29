#pragma once

#include "../../gaEngine/gaBehaviorNode/gaBehaviorSound.h"

namespace DarkForces {
	namespace Behavior {
		class Sound : public GameEngine::Behavior::Sound {
		public:
			Sound(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			void addSound(const char* file, uint32_t id) override;
		};
	}
}