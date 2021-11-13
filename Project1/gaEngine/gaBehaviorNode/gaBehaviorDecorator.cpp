#include "gaBehaviorDecorator.h"

#include <imgui.h>

GameEngine::Behavior::Decorator::Decorator(const char *name) : 
	BehaviorNode(name)
{
}

void GameEngine::Behavior::Decorator::execute(Action* r)
{
	// exit the node if the init part failed
	if (m_status != Status::RUNNING) {
		return BehaviorNode::execute(r);
	}

	// at first execution immediately run the child
	if (m_runningChild < 0) {
		m_runningChild = 0;
		return startChild(r, m_runningChild, m_data);
	}

	Status childStatus = m_children[m_runningChild]->status();
	switch (childStatus) {
	case Status::SUCCESSED:
	case Status::FAILED:
		onChildExit(m_runningChild, childStatus);

		// drop out of the loop
		r->action = BehaviorNode::Status::EXIT;
		switch (m_condition) {
		case Condition::STRAIGHT:
			m_status = childStatus;
			break;
		case Condition::INVERT:
			m_status = (childStatus == Status::SUCCESSED) ? Status::FAILED : Status::SUCCESSED;
			break;
		case Condition::FAILURE:
			m_status = Status::FAILED;
			break;
		default:
			m_status = Status::SUCCESSED;
		}
		r->status = m_status;
		break;

	default:
		r->action = BehaviorNode::Status::RUNNING;
		break;
	}
}

/**
 * display the node data in the debugger
 */
void GameEngine::Behavior::Decorator::debugGUInode(void)
{
	static const char* conditions[] = {
		"Straight",
		"Inverse",
		"Failure",
		"Sucess"
	};
	ImGui::Text("Decorator:%s", conditions[(uint32_t)m_condition]);
}
