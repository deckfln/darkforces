#pragma once

#include <vector>
#include "../../gaEngine/gaBehaviorNode/gaBehaviorDecorator.h"

class gaEntity;

namespace DarkForces {
	namespace Behavior {
		class Move2Player : public GameEngine::Behavior::Decorator
		{
			glm::vec3 m_target;												// move
			uint32_t m_alarmID=0;											// alarm programmed to cancel the move node
			uint32_t m_maximum_walk = 2000;
			uint32_t m_minimum_walk = 2000;
			bool m_random = false;
			uint32_t m_walk = 2000;											// time to walk before dropping out

		protected:
			void onChildExit(uint32_t child, Status status) override;

		public:
			Move2Player(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void init(void* data) override;									// init the node before running

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			//debugger
			void debugGUInode(void) override;							// display the component in the debugger
		};
	}
}
