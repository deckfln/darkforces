#pragma once

#include "../../gaEngine/gaBehaviorNode.h"

class gaEntity;

namespace DarkForces {
	namespace Behavior {
		class Fire2Player : public GameEngine::BehaviorNode
		{
			glm::vec3 m_position;											// player position
			glm::vec3 m_direction;											// direction to the last player position
			uint32_t m_state = 0;
			uint32_t m_firingFrames = 0;
			uint32_t m_firingFrame = 0;

			glm::vec3 m_from;											// player position
			glm::vec3 m_to;											// player position
			bool m_visibility;

			void fireNow(void);

		public:
			Fire2Player(const char* name);
			void init(void* data) override;									// init the node before running
			void dispatchMessage(gaMessage* message, Action* r) override;	// let a component deal with a situation

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			//debugger
			void debugGUInode(void) override;							// display the component in the debugger

			// flight recorder status
			uint32_t recordState(void* record) override;				// save the component state in a record
			uint32_t loadState(void* record) override;					// reload a component state from a record
		};
	}
}
