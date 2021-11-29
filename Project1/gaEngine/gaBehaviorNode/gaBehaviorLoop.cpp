#include "gaBehaviorLoop.h"

#include <imgui.h>
#include <tinyxml2.h>

GameEngine::Behavior::Loop::Loop(const char *name) : 
	BehaviorNode(name)
{
}

GameEngine::BehaviorNode* GameEngine::Behavior::Loop::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	return new GameEngine::Behavior::Loop(name);
}

void GameEngine::Behavior::Loop::execute(Action* r)
{
	// exit if init changed the status
	switch (m_status) {
	case Status::FAILED:
		return failed(r);
	case Status::SUCCESSED:
		return succeeded(r);
	}

	// if this is the first run, stat child 0
	if (m_runningChild == -1) {
		m_runningChild = 0;
		onChildStart(m_runningChild);
		return startChild(r, m_runningChild, m_data);
	}

	onChildExit(m_runningChild, m_children[m_runningChild]->status());

	// we are in the default loop
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
			onChildStart(m_runningChild);
			return startChild(r, m_runningChild, m_data);
		}

		// drop out of the loop if all failed
		r->action = BehaviorNode::Status::EXIT;
		m_status  = r->status = BehaviorNode::Status::FAILED;
		break;}

	case Condition::UNTIL_ONE_FAIL: {
		switch (m_children[m_runningChild]->status()) {
		case Status::SUCCESSED:
			// loop over the nodes
			m_runningChild++;
			if (m_runningChild >= m_children.size()) {
				m_runningChild = 0;
			}
			onChildStart(m_runningChild);
			return startChild(r, m_runningChild, m_data);
			break;

		case Status::FAILED:
			// drop out of the loop
			r->action = BehaviorNode::Status::EXIT;
			m_status = r->status = BehaviorNode::Status::FAILED;
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
	switch (m_condition) {
	case Condition::UNTIL_ALL_FAIL:
		ImGui::Text("Loop until all fail");
		break;

	case Condition::UNTIL_ONE_FAIL:
		ImGui::Text("Loop until one fail");
		break;
	}
}
