#include "gaBehaviorSequence.h"

#include <imgui.h>
#include <tinyxml2.h>

static std::map<const char*, GameEngine::Behavior::Sequence::Condition> g_conditions = {
	{"exit_if_fail", GameEngine::Behavior::Sequence::Condition::EXIT_WHEN_ONE_FAIL},
	{"exit_first_success", GameEngine::Behavior::Sequence::Condition::EXIT_FIRST_SUCCESS},
	{"even_if_fail", GameEngine::Behavior::Sequence::Condition::EXIT_AT_END},
};
static const char* g_className = "Sequence";

GameEngine::Behavior::Sequence::Sequence(const char *name) : 
	BehaviorNode(name)
{
	m_className = g_className;
}

GameEngine::BehaviorNode* GameEngine::Behavior::Sequence::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::Sequence* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::Sequence*>(p);
	}
	else {
		cl = new GameEngine::Behavior::Sequence(m_name);
	}
	GameEngine::BehaviorNode::clone(cl);
	cl->m_condition = m_condition;
	return cl;
}

GameEngine::BehaviorNode* GameEngine::Behavior::Sequence::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::Sequence* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::Sequence(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::Sequence*>(used);
	}
	GameEngine::BehaviorNode::create(name, element, node);

	tinyxml2::XMLElement* attr = element->FirstChildElement("condition");
	if (attr) {
		const char* t = attr->GetText();
		for (auto& c : g_conditions) {
			if (strcmp(t, c.first) == 0) {
				node->condition(c.second);
				break;
			}
		}
	}
	return node;
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

	if (status == Status::ERR) {
		return error(r);
	}

	// if the current node fails, it depends on the exit condition
	if (status == Status::FAILED) {
		if (m_condition == Condition::EXIT_WHEN_ONE_FAIL) {
			// drop out of the loop
			return failed(r);
		}
		else {
			m_failed++;
		}
	}
	else if (status == Status::SUCCESSED) {
		if (m_condition == Condition::EXIT_FIRST_SUCCESS) {
			// drop out of the loop
			return succeeded(r);
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
	switch (m_condition) {
	case Condition::EXIT_WHEN_ONE_FAIL:
		ImGui::Text("unless one fail");
		break;
	case Condition::EXIT_FIRST_SUCCESS:
		ImGui::Text("until one success");
		break;
	case Condition::EXIT_AT_END:
		ImGui::Text("to the end");
		break;
	}
}
