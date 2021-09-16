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
		BehaviorNode(void);

		// getter/setter
		inline BehaviorNode* parent(void) { return m_parent; };
		inline Status status(void) { return m_status; };
		inline bool isSequence(void) { return m_sequence; };
		inline void tree(Component::BehaviorTree* tree) { m_tree = tree; };
		void instanciate(gaEntity* entity);

		BehaviorNode* addNode(BehaviorNode*);

		BehaviorNode* startChild(int32_t child, void* data);		// move to a sub-node
		BehaviorNode* exitChild(Status s);							// return to parent node

		virtual BehaviorNode *nextNode(void);						// let a parent take a decision with it's current running child
		virtual BehaviorNode* dispatchMessage(gaMessage* message);	// let a component deal with a situation
		virtual void init(void *);									// init the node before running

		void sendInternalMessage(int action,
			const glm::vec3& value);								// send internal message to all components of the current entity

	protected:
		Status m_status = Status::WAIT;
		BehaviorNode* m_parent = nullptr;
		gaEntity* m_entity = nullptr;								// entity this node belongs to
		Component::BehaviorTree* m_tree = nullptr;					// tree this node belongs to
		bool m_sequence = false;

		int32_t m_runningChild = -1;								// currently running child (-1 = the current node is running)
		std::vector<BehaviorNode*> m_children;
	};
}