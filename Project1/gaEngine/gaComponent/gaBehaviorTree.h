#pragma once

#include <map>
#include "../gaComponent.h"

namespace GameEngine {
	class BehaviorNode;

	namespace Component {
		class BehaviorTree : public gaComponent {
			uint32_t m_nbnodes=0;								// number of nodes in the tree
			BehaviorNode* m_root = nullptr;
			BehaviorNode* m_current = nullptr;
			bool m_instanciated = false;

			std::map<std::string, void*> m_blackboard;
			std::vector<BehaviorNode*> m_nodes;					// index of all nodes

		public:
			BehaviorTree(BehaviorNode* root);
			void* blackboard(const std::string key);
			void blackboard(const std::string key, void *value);

			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation

			// flight recorder status
			inline uint32_t recordSize(void);					// size of the component record
			uint32_t recordState(void* record);					// save the component state in a record
			uint32_t loadState(void* record);					// reload a component state from a record

			// debugger
			void debugGUIinline(void) override;					// display the component in the debugger
		};
	}
}
