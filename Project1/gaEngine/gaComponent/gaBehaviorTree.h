#pragma once

#include <map>
#include <deque>
#include "../../include/imnodes.h"

#include "../gaComponent.h"

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

			template <typename T>
			void blackboard(const std::string key, const T& value);

			template <typename T>
			void blackboard(const std::string key, const T* value);

			template <typename T>
			T& blackboard(const std::string key);

			void dispatchMessage(gaMessage* message) override;	// let a component deal with a situation

			inline uint32_t lastAttrId(void) { return m_lastId++; };
			inline uint32_t lastNode(void) { return m_lastNode; };

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

			std::map<std::string, void*> m_blackboard;
			std::vector<BehaviorNode*> m_nodes;					// index of all nodes

			std::map<uint32_t, msgHandler> m_handlers;
			static uint32_t m_lastId;
			static uint32_t m_lastNode;

#ifdef _DEBUG
			ImNodesEditorContext* m_context = nullptr;
			bool m_debug = false;
#endif // _DEBUG

		};

		template<typename T>
		inline void BehaviorTree::blackboard(const std::string key, const T& value)
		{
			if (m_blackboard[key] == nullptr) {
				m_blackboard[key] = new T;
			}

			*(static_cast<T*>(m_blackboard[key])) = value;
		}

		template<typename T>
		inline void BehaviorTree::blackboard(const std::string key, const T* value)
		{
			if (m_blackboard[key] == nullptr) {
				m_blackboard[key] = new T;
			}
			m_blackboard[key] = (void*)value;
		}

		template<typename T>
		inline T& BehaviorTree::blackboard(const std::string key)
		{
			if (m_blackboard[key] == nullptr) {
				m_blackboard[key] = new T;
			}

			return *(static_cast<T*>(m_blackboard[key]));
		}
	}
}
