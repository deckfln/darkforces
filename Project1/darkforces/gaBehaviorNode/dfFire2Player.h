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

#ifdef _DEBUG
			glm::vec3 m_from;											// actor position
			glm::vec3 m_to;												// target position
#endif

			void fireNow(void);

		public:
			Fire2Player(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;

			void init(void* data) override;									// init the node before running
			void dispatchMessage(gaMessage* message, Action* r) override;	// let a component deal with a situation

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

#ifdef _DEBUG
			//debugger
			void debugGUInode(GameEngine::Component::BehaviorTree* tree) override;		// display the component in the debugger
#endif

			// flight recorder status
			uint32_t recordState(void* record) override;				// save the component state in a record
			uint32_t loadState(void* record) override;					// reload a component state from a record
		};
	}
}
