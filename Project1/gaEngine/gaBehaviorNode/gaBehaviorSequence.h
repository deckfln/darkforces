#pragma once

#include "../gaBehaviorNode.h"

namespace GameEngine {
	namespace Behavior {
		class Sequence : public BehaviorNode {
		public:
			enum class Condition {
				EXIT_WHEN_ONE_FAIL,
				EXIT_AT_END
			};

			Sequence(const char* name);
			inline void condition(Condition c) { m_condition = c; };
			void init(void* data) override;
			void execute(Action* r) override;						// let a parent take a decision with it's current running child

			// Behavior engine
			static BehaviorNode* create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used);// create a node

			// debugger
			void debugGUInode(void) override;

		protected:
			Condition m_condition = Condition::EXIT_WHEN_ONE_FAIL;

		private:
			uint32_t m_failed = 0;
		};
	}
}