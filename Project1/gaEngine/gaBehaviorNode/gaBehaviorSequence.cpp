#include "gaBehaviorSequence.h"

#include <imgui.h>
#include <tinyxml2.h>

GameEngine::Behavior::Sequence::Sequence(const char *name) : 
	BehaviorNode(name)
{
}

GameEngine::BehaviorNode* GameEngine::Behavior::Sequence::create(const char* name, tinyxml2::XMLElement* element)
{
	return new GameEngine::Behavior::Sequence(name);
}

void GameEngine::Behavior::Sequence::init(void* data)
{
	m_failed = 0;
	BehaviorNode::init(data);
}

void GameEngine::Behavior::Sequence::execute(Action* r)
{
	// exit if init changed the status
	switch (m_status) {
	case Status::FAILED:
		return failed(r);
	case Status::SUCCESSED:
		return succeeded(r);
	}

	// when run at first
	if (m_runningChild == -1) {
		m_runningChild = 0;
		onChildStart(m_runningChild);
		return startChild(r, m_runningChild, m_data);
	}

	Status status = m_children[m_runningChild]->status();
	onChildExit(m_runningChild, status);

	// if the current node fails, it depends on the exit condition
	if (status == Status::FAILED) {
		if (m_condition == Condition::EXIT_WHEN_ONE_FAIL) {
			// drop out of the loop
			r->action = BehaviorNode::Status::EXIT;
			r->status = Status::FAILED;
			return;
		}
		else {
			m_failed++;
		}
	}

	// Move to the next node in the sequence
	m_runningChild++;
	if (m_runningChild >= m_children.size()) {
		// until the last node in the sequence
		return m_failed == 0 ? succeeded(r) : failed(r);
	}

	onChildStart(m_runningChild);
	return startChild(r, m_runningChild, m_data);
}

/**
 * display the node data in the debugger
 */
void GameEngine::Behavior::Sequence::debugGUInode(void)
{
	if (m_condition == Condition::EXIT_WHEN_ONE_FAIL) {
		ImGui::Text("Sequence unless one fail");
	}
	else {
		ImGui::Text("Sequence even if one fail");
	}
}
