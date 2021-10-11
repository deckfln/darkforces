#include "gaBehaviorLoop.h"

#include <imgui.h>

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

	switch (m_condition) {
	case Condition::UNTIL_ALL_FAIL: {
		// check if all children failed
		uint32_t failed = true;
		for (auto& child : m_children) {
			if (child->status() != Status::FAILED) {
				failed = !failed;
				break;
			}
		}

		// if one succeeded continue the loop
		if (!failed) {
			// loop over the nodes
			m_runningChild++;
			if (m_runningChild >= m_children.size()) {
				m_runningChild = 0;
			}
			return startChild(r, m_runningChild, m_data);
		}

		// drop out of the loop if all failed
		r->action = BehaviorNode::Status::EXIT;
		r->status = m_status;
		break;}

	case Condition::UNTIL_ONE_FAIL: {
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
		}}
	}
}

/**
 * display the node data in the debugger
 */
void GameEngine::Behavior::Loop::debugGUInode(void)
{
	ImGui::Text("Loop");
}
