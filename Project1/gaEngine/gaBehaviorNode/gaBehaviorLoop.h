#pragma once

#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class Loop : public BehaviorNode {
		public:
			enum class Condition {
				UNTIL_ONE_FAIL,
				UNTIL_ONE_SUCCESS,
				UNTIL_ALL_FAIL,
				UNTIL_ALL_SUCCCES
			};
				
			Loop(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;

			inline void condition(Condition c) { m_condition = c; };
			void execute(Action* r) override;						// let a parent take a decision with it's current running child
			void dispatchMessage(gaMessage* message, Action* r) override;	// let a component deal with a situation

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

#ifdef _DEBUG
			//debugger
			void debugGUInode(GameEngine::Component::BehaviorTree* tree) override;	// display the component in the debugger
#endif
		protected:
			Condition m_condition = Condition::UNTIL_ALL_FAIL;
			virtual bool endLoop(void);
		};
	}
}