#include "gaBehaviorSequence.h"

#include <imgui.h>

GameEngine::Behavior::Sequence::Sequence(const char *name) : 
	BehaviorNode(name)
{
}

void GameEngine::Behavior::Sequence::init(void* data)
{
	m_runningChild = -1;
	BehaviorNode::init(data);
}

void GameEngine::Behavior::Sequence::execute(Action* r)
{
	if (m_runningChild == -1) {
		m_runningChild = 0;
		return startChild(r, m_runningChild, m_data);
	}

	switch (m_children[m_runningChild]->status()) {
	case Status::SUCCESSED:
		// Move to the next node in the sequence
		m_runningChild++;
		if (m_runningChild >= m_children.size()) {
			// until the last node in the sequence
			return succeeded(r);
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

/**
 * display the node data in the debugger
 */
void GameEngine::Behavior::Sequence::debugGUInode(void)
{
	ImGui::Text("Sequence");
}
