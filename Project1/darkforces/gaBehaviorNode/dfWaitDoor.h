#pragma once

#include "../../gaEngine/gaBehaviorNode.h"
#include "../../gaEngine/gaVariable.h"

namespace DarkForces {
	namespace Component {
		class InfElevator;
	}

	namespace Behavior {
		class WaitDoor : public GameEngine::BehaviorNode
		{
			GameEngine::Variable m_elevator;

		public:
			WaitDoor(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;
			void dispatchMessage(gaMessage* message, Action* r) override;	// let a component deal with a situation
			void init(void* data) override;

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			//debugger
			void debugGUInode(GameEngine::Component::BehaviorTree* tree) override;	// display the component in the debugger
		};
	}
}
