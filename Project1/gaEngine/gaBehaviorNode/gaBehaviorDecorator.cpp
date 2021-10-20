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
		onChildExit(childStatus);

		// drop out of the loop
		m_status = m_children[m_runningChild]->status();
		r->action = BehaviorNode::Status::EXIT;
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
	ImGui::Text("Decorator");
}
