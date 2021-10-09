#include "../gaBehaviorNode.h"
#include "gaBehaviorLoop.h"

GameEngine::Behavior::Loop::Loop(const char *name) : 
	BehaviorNode(name)
{
}

void GameEngine::Behavior::Loop::execute(Action* r)
{
	if (m_runningChild == -1) {
		m_runningChild = 0;
		return startChild(r, m_runningChild, m_data);
	}

	switch (m_children[m_runningChild]->status()) {
	case Status::SUCCESSED:
		// loop over the nodes
		m_runningChild++;
		if (m_runningChild >= m_children.size()) {
			m_runningChild = 0;
		}
		return startChild(r, m_runningChild, m_data);
		break;

	case Status::FAILED:
		// drop out of the loop
		r->action = BehaviorNode::Status::EXIT;
		r->status = m_status;
		break;

	default:
		r->action = BehaviorNode::Status::RUNNING;
		break;
	}
}
