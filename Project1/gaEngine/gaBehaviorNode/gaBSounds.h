#pragma once

#include "gaBSetVar.h"

namespace GameEngine {
	namespace Behavior {
		class Sounds : public GameEngine::Behavior::SetVar
		{
		public:
			struct Origin {
				glm::vec3 m_position;
				float m_loundness;
			};

			Sounds(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void init(void*) override;

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node
		};
	}
}
