#include "gaBFor.h"

#include <imgui.h>
#include <tinyxml2.h>

static const char* g_className = "For";

GameEngine::Behavior::For::For(const char *name) : 
	GameEngine::Behavior::Loop(name)
{
	m_className = g_className;
}

GameEngine::BehaviorNode* GameEngine::Behavior::For::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::For* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::For*>(p);
	}
	else {
		cl = new GameEngine::Behavior::For(m_name);
	}
	GameEngine::Behavior::Loop::clone(cl);
	cl->m_variable = m_variable;
	cl->m_start = m_start;
	cl->m_end = m_end;
	return cl;
}

GameEngine::BehaviorNode* GameEngine::Behavior::For::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::For* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::For(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::For*>(used);
	}
	GameEngine::Behavior::Loop::create(name, element, node);

	tinyxml2::XMLElement* xmlVar = element->FirstChildElement("variable");
	if (xmlVar) {
		node->m_variable = xmlVar->GetText();
	}

	xmlVar = element->FirstChildElement("start");
	node->m_start.set(xmlVar);

	xmlVar = element->FirstChildElement("end");
	node->m_end.set(xmlVar);

	xmlVar = element->FirstChildElement("default_return");
	const char* returndefault = xmlVar->GetText();
	if (returndefault != nullptr) {
		if (strcmp(returndefault, "failure") == 0) {
			node->m_defaultreturn = Status::FAILED;
		}
		else if (strcmp(returndefault, "success") == 0) {
			node->m_defaultreturn = Status::SUCCESSED;
		}
	}

	return node;
}

//----------------------------------------

/**
 *
 */
void GameEngine::Behavior::For::init(void* data)
{
	int32_t& counter = m_tree->blackboard().get<int32_t>(m_variable, GameEngine::Variable::Type::INT32);
	m_start.get(counter, m_tree);
}

/**
 * let a parent take a decision with it's current running child
 */
bool GameEngine::Behavior::For::endLoop(void)
{
	bool b = GameEngine::Behavior::Loop::endLoop();

	if (!b) {
		// end of the loop without exit
		// increase the counter at the end of each loop
		int32_t& counter = m_tree->blackboard().get<int32_t>(m_variable, GameEngine::Variable::Type::INT32);
		int32_t end;
		m_end.get(end, m_tree);

		counter++;

		if (counter >= end) {
			m_status = m_defaultreturn;
			return true;
		}
	}

	return b;
}
