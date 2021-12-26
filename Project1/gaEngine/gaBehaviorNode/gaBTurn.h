#pragma once

#include <vector>
#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class Turn : public BehaviorNode {
			std::vector<float> m_angles;						// list of angles to turn to
			uint32_t m_currentAngle=0;							// current one
			uint32_t m_delay=0;									// time to spend on each angle
			uint32_t m_untilNextTurn=0;							// frames left until next turn
			std::map<std::string, bool> m_exit;					// list of variables triggering exit of the node

			bool conditionMet(void);
			void onTimer(gaMessage* message);					// move around

		public:
			Turn(const char* name);

			void dispatchMessage(gaMessage* message, Action* r) override;	// let a component deal with a situation
			void init(void*) override;

			// Behavior engine, GameEngine::BehaviorNode* used
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			//debugger
			void debugGUInode(void) override;							// display the component in the debugger

			// flight recorder status
			uint32_t recordState(void* record) override;				// save the component state in a record
			uint32_t loadState(void* record) override;					// reload a component state from a record
		};
	}

	namespace FlightRecorder {
		struct Turn {
			BehaviorNode node;
			uint32_t current;
			uint32_t untilNextTurn;
		};
	}
}