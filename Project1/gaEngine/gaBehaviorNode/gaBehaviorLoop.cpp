#include "gaBehaviorLoop.h"

#include <imgui.h>
#include <tinyxml2.h>

static std::map<const char*, GameEngine::Behavior::Loop::Condition> g_conditions = {
	{"until_all_fail", GameEngine::Behavior::Loop::Condition::UNTIL_ALL_FAIL},		// return the status of the child
	{"until_one_fail", GameEngine::Behavior::Loop::Condition::UNTIL_ONE_FAIL},			// return the inverse of the child status
};

GameEngine::Behavior::Loop::Loop(const char *name) : 
	BehaviorNode(name)
{
}

GameEngine::BehaviorNode* GameEngine::Behavior::Loop::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::Loop* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::Loop*>(p);
	}
	else {
		cl = new GameEngine::Behavior::Loop(m_name);
	}
	cl->m_condition = m_condition;
	return cl;
}

GameEngine::BehaviorNode* GameEngine::Behavior::Loop::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::Loop* node = new GameEngine::Behavior::Loop(name);
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

/**
 *
 */
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
		return startChild(r, m_runningChild, r->data);
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
			return startChild(r, m_runningChild, r->data);
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
 * if the loop is running alone, force it to execute the children
 */
void GameEngine::Behavior::Loop::dispatchMessage(gaMessage* message, Action* r)
{
	r->action = BehaviorNode::Status::EXECUTE;
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
