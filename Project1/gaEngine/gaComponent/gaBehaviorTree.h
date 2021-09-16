#pragma once

#include <map>
#include "../gaComponent.h"

namespace GameEngine {
	class BehaviorNode;

	namespace Component {
		class BehaviorTree : public gaComponent {
			BehaviorNode* m_root = nullptr;
			BehaviorNode* m_current = nullptr;
			bool m_instanciated = false;

			std::map<std::string, void*> m_blackboard;

		public:
			BehaviorTree(BehaviorNode* root);
			void* blackboard(const std::string key);
			void blackboard(const std::string key, void *value);

			void dispatchMessage(gaMessage* message) override;		// let a component deal with a situation

			// debugger
			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}
