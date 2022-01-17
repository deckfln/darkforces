#pragma once

#include <map>
#include <deque>
#ifdef _DEBUG
#include "../../include/imnodes.h"
#endif

#include "../gaComponent.h"
#include "../gaBlackboard.h"

namespace GameEngine {
	class BehaviorNode;

	namespace Component {
		class BehaviorTree : public gaComponent {
		public:
			typedef bool(GameEngine::Component::BehaviorTree::*msgHandler) (gaMessage*);//pointer-to-member function

			BehaviorTree(void);
			BehaviorTree(BehaviorNode* root);
			~BehaviorTree(void);

			void parse(const std::string& data,
				const std::map<std::string, std::string>& includes);// create a tree from XML data

			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation

			inline uint32_t lastAttrId(void) { return m_lastId++; };
			inline uint32_t lastNode(void) { return m_lastNode; };
			inline GameEngine::Blackboard& blackboard(void) { return m_blackboard; };

			void handlers(uint32_t, msgHandler);				// manager message handlers


			// preset message handlers
			bool onViewPlayer(gaMessage*);						// player is viewed
			bool onNotViewPlayer(gaMessage*);					// player is not viewed
			bool onHearSound(gaMessage*);						// hear a sound
			bool onBulletHit(gaMessage*);						// hit by a bullet

			// flight recorder status
			inline uint32_t recordSize(void);					// size of the component record
			uint32_t recordState(void* record);					// save the component state in a record
			uint32_t loadState(void* record);					// reload a component state from a record

			// debugger
			void debugGUIinline(void) override;					// display the component in the debugger

		protected:
			uint32_t m_nbnodes = 0;								// number of nodes in the tree
			BehaviorNode* m_root = nullptr;
			BehaviorNode* m_current = nullptr;
			bool m_instanciated = false;
			bool m_active = false;								// AI is activated or not

			std::vector<BehaviorNode*> m_nodes;					// index of all nodes

			std::map<uint32_t, msgHandler> m_handlers;
			static uint32_t m_lastId;
			static uint32_t m_lastNode;

#ifdef _DEBUG
			ImNodesEditorContext* m_context = nullptr;
			bool m_debug = false;
#endif // _DEBUG

			// blackboard
			GameEngine::Blackboard m_blackboard;
		};
	}
}
