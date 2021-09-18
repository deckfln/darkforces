#pragma once

#include <vector>
#include "gaMessage.h"

/*
 * Define a default node in a Behavior Tree
 */

class gaEntity;

namespace GameEngine {
	namespace Component {
		class BehaviorTree;
	};

	class BehaviorNode {
	public:
		enum class Status {
			WAIT,
			RUNNING,
			FAILED,
			SUCCESSED
		};
		BehaviorNode(const char *name);

		// getter/setter
		inline BehaviorNode* parent(void) { return m_parent; };
		inline Status status(void) { return m_status; };
		inline bool isSequence(void) { return m_sequence; };
		inline void tree(Component::BehaviorTree* tree) { m_tree = tree; };
		inline const char* name(void) { return m_name; };
		inline uint32_t id(void) { return m_id; };

		void instanciate(gaEntity* entity);

		BehaviorNode* addNode(BehaviorNode*);						// add a sub-node to that node
		uint32_t count(uint32_t id);								// count nodes in a tree a register ID
		BehaviorNode* find(uint32_t id);							// find the node with ID
		void record(std::vector<BehaviorNode*>& nodes);				// record the node in a list

		BehaviorNode* startChild(int32_t child, void* data);		// move to a sub-node
		BehaviorNode* exitChild(Status s);							// return to parent node

		virtual BehaviorNode *nextNode(void);						// let a parent take a decision with it's current running child
		virtual BehaviorNode* dispatchMessage(gaMessage* message);	// let a component deal with a situation
		virtual void init(void *);									// init the node before running

			// debugger
		virtual void debugGUIinline(BehaviorNode* current);			// display the component in the debugger


			// flight recorder status
		virtual uint32_t recordState(void* record);					// save the component state in a record
		virtual uint32_t loadState(void* record);					// reload a component state from a record

		void sendInternalMessage(int action,
			const glm::vec3& value);								// send internal message to all components of the current entity

	protected:
		uint32_t m_id = 0;											// uniq ID of the node in the tree
		const char* m_name = nullptr;
		Status m_status = Status::WAIT;
		BehaviorNode* m_parent = nullptr;
		gaEntity* m_entity = nullptr;								// entity this node belongs to
		Component::BehaviorTree* m_tree = nullptr;					// tree this node belongs to
		bool m_sequence = false;

		int32_t m_runningChild = -1;								// currently running child (-1 = the current node is running)
		std::vector<BehaviorNode*> m_children;
	};

	namespace FlightRecorder {
		struct BehaviorNode {
			uint32_t size;
			uint32_t id;
			uint32_t status;
			uint32_t runningChild;
		};
	}
}