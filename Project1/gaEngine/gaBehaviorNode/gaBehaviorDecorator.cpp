#include "gaBehaviorDecorator.h"

#include <imgui.h>
#include <tinyxml2.h>
#include <map>

static std::map<const char*, GameEngine::Behavior::Decorator::Condition> g_conditions = {
	{"straight", GameEngine::Behavior::Decorator::Condition::STRAIGHT},		// return the status of the child
	{"invert", GameEngine::Behavior::Decorator::Condition::INVERT},			// return the inverse of the child status
	{"false", GameEngine::Behavior::Decorator::Condition::FAILURE},		// always return failure
	{"true", GameEngine::Behavior::Decorator::Condition::SUCESS}			// always return success
};

/**
 * Create a node
 */
GameEngine::Behavior::Decorator::Decorator(const char *name) :
	BehaviorNode(name)
{
}

GameEngine::BehaviorNode* GameEngine::Behavior::Decorator::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::Decorator* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::Decorator*>(p);
	}
	else {
		cl = new GameEngine::Behavior::Decorator(m_name);
	}
	GameEngine::BehaviorNode::clone(cl);
	cl->m_condition = m_condition;
	return cl;
}

GameEngine::BehaviorNode* GameEngine::Behavior::Decorator::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::Decorator* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::Decorator(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::Decorator*>(used);
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

	case Status::ERR:
		if (m_continueOnError) {
			return failed(r);
		}
		return error(r);

	default:
		r->action = BehaviorNode::Status::RUNNING;
		break;
	}
}

/**
 * Debugger
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
