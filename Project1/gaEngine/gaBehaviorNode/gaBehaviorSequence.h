#pragma once

#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class Sequence : public BehaviorNode {
		public:
			enum class Condition {
				EXIT_WHEN_ONE_FAIL,
				EXIT_AT_END,
				EXIT_FIRST_SUCCESS
			};

			Sequence(const char* name);
			BehaviorNode* clone(GameEngine::BehaviorNode* p) override;

			inline void condition(Condition c) { m_condition = c; };
			void init(void* data) override;
			void execute(Action* r) override;						// let a parent take a decision with it's current running child

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			// debugger
			void debugGUInode(GameEngine::Component::BehaviorTree* tree) override;

		protected:
			Condition m_condition = Condition::EXIT_WHEN_ONE_FAIL;

		private:
			uint32_t m_failed = 0;
		};
	}
}