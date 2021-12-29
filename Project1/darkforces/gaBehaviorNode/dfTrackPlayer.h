#pragma once

#include <vector>
#include <imgui.h>

#include "../../gaEngine/gaBehaviorNode/gaBehaviorDecorator.h"

namespace DarkForces {
	namespace Behavior {
		class TrackPlayer : public GameEngine::Behavior::Decorator {
			glm::vec3 m_target;								// navigation for satnav

		public:
			TrackPlayer(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void init(void* data) override;									// init the node before running

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			//debugger
			void debugGUInode(void) override;							// display the component in the debugger
		};
	}
}