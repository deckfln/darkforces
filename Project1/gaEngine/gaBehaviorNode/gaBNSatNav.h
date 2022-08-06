#pragma once

#include "gaMoveTo.h"
#include "../gaVariable.h"

class gaEntity;

namespace GameEngine {
	namespace Behavior {
		class SatNav : public MoveTo {
			bool m_move2variable = false;						// move to a position given directly or to a variable on the blackboard

			glm::vec3 m_destination;

			Variable m_destName;

			std::vector<glm::vec3> m_navpoints;					// nav points for the move

			void onGoto(gaMessage* message);

		public:
			SatNav(void);
			SatNav(const char* name);
			SatNav(const char* name, float speed);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;

			void dispatchMessage(gaMessage* message, Action *r) override;	// let a component deal with a situation
			void init(void *) override;

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

#ifdef _DEBUG
			// debugger
			void debugGUInode(GameEngine::Component::BehaviorTree* tree) override;
			void debugConsoleIn(GameEngine::Component::BehaviorTree* tree) override;	// display on the console
			void debugConsoleOut(GameEngine::Component::BehaviorTree* tree) override;	// display on the console
#endif

			// flight recorder status
			uint32_t recordState(void* record) override;				// save the component state in a record
			uint32_t loadState(void* record) override;					// reload a component state from a record
		};
	}

	namespace FlightRecorder {
		struct SatNav {
			FlightRecorder::MoveTo move2;
			glm::vec3 destination;
			uint32_t nbNavPoints;
			glm::vec3 points[1];
		};
	}
}