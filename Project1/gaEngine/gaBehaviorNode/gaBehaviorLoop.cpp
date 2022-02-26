#include "gaBehaviorLoop.h"

#include <imgui.h>
#include <tinyxml2.h>

static std::map<const char*, GameEngine::Behavior::Loop::Condition> g_conditions = {
	{"until_all_fail", GameEngine::Behavior::Loop::Condition::UNTIL_ALL_FAIL},
	{"until_one_fail", GameEngine::Behavior::Loop::Condition::UNTIL_ONE_FAIL},
	{"until_one_success", GameEngine::Behavior::Loop::Condition::UNTIL_ONE_SUCCESS},
	{"until_all_success", GameEngine::Behavior::Loop::Condition::UNTIL_ALL_SUCCCES}
};

static const char* g_className = "Loop";

GameEngine::Behavior::Loop::Loop(const char *name) : 
	BehaviorNode(name)
{
	m_className = g_className;
}

/**
 *
 */
GameEngine::BehaviorNode* GameEngine::Behavior::Loop::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::Loop* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::Loop*>(p);
	}
	else {
		cl = new GameEngine::Behavior::Loop(m_name);
	}
	GameEngine::BehaviorNode::clone(cl);
	cl->m_condition = m_condition;
	return cl;
}

/**
 *
 */
GameEngine::BehaviorNode* GameEngine::Behavior::Loop::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::Loop* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::Loop(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::Loop*>(used);
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

/**
 *
 */
bool GameEngine::Behavior::Loop::endLoop(void)
{
	Status status;
	uint32_t fail=0;
	uint32_t sucess = 0;

	for (auto& child : m_children) {
		status = child->status();
		switch (status) {
		case Status::FAILED:
			fail++;
			break;
		case Status::SUCCESSED:
			sucess++;
			break;
		}
	}

	if (m_condition == Condition::UNTIL_ALL_FAIL) {
		if (fail == m_children.size()) {
			m_status = Status::FAILED;
			return true;
		}
	}
	else if (m_condition == Condition::UNTIL_ALL_SUCCCES) {
		if (sucess == m_children.size()) {
			m_status = Status::SUCCESSED;
			return true;
		}
	}
	else {
		exit(-1);
	}

	return false;
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

	Status status = m_children[m_runningChild]->status();
	if (status == Status::ERR) {
		return error(r);
	}

	// we are in the default loop
	switch (m_condition) {
	case Condition::UNTIL_ALL_FAIL:
	case Condition::UNTIL_ALL_SUCCCES:
		// loop over the nodes
		m_runningChild++;
		if (m_runningChild >= m_children.size()) {
			m_runningChild = 0;
			if (endLoop()) {
				// handle end of the loop
				return GameEngine::BehaviorNode::execute(r);
			}		
		}
		onChildStart(m_runningChild);
		return startChild(r, m_runningChild, r->data);

	case Condition::UNTIL_ONE_FAIL:
		switch (status) {
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
			return failed(r);
			break;

		default:
			r->action = BehaviorNode::Status::RUNNING;
			break;
		}

	case Condition::UNTIL_ONE_SUCCESS:
		switch (status) {
		case Status::FAILED:
			// loop over the nodes
			m_runningChild++;
			if (m_runningChild >= m_children.size()) {
				m_runningChild = 0;
			}
			onChildStart(m_runningChild);
			return startChild(r, m_runningChild, m_data);
			break;

		case Status::SUCCESSED:
			// drop out of the loop
			return succeeded(r);
			break;

		default:
			r->action = BehaviorNode::Status::RUNNING;
			break;
		}
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
void GameEngine::Behavior::Loop::debugGUInode(GameEngine::Component::BehaviorTree* tree)
{
	switch (m_condition) {
	case Condition::UNTIL_ALL_FAIL:
		ImGui::Text("until all fail");
		break;

	case Condition::UNTIL_ONE_FAIL:
		ImGui::Text("until one fail");
		break;

	case Condition::UNTIL_ALL_SUCCCES:
		ImGui::Text("until all success");
		break;

	case Condition::UNTIL_ONE_SUCCESS:
		ImGui::Text("until one success");
		break;
	}
}
