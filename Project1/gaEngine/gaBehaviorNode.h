#pragma once

#include <vector>
#include "gaMessage.h"

/*
 * Define a default node in a Behavior Tree
 */

class gaEntity;

namespace GameEngine {
	class BehaviorNode {
	public:
		enum class Status {
			STILL,
			RUNNING,
			FAILED,
			SUCCESSED
		};
		BehaviorNode(void);

		// getter/setter
		inline BehaviorNode* parent(void) { return m_parent; };
		inline Status status(void) { return m_status; };
		void instanciate(gaEntity* entity);

		BehaviorNode* addNode(BehaviorNode*);
		virtual BehaviorNode *nextNode(void);						// let a parent take a decision with it's current running child
		virtual void dispatchMessage(gaMessage* message);			// let a component deal with a situation

	protected:
		Status m_status = Status::RUNNING;
		BehaviorNode* m_parent = nullptr;
		int32_t m_runningChild = -1;								// currently running child (-1 = the current node is running)
		std::vector<BehaviorNode*> m_children;
		gaEntity* m_entity = nullptr;
	};
}