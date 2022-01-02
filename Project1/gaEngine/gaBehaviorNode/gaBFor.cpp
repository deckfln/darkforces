#include "gaBFor.h"

#include <imgui.h>
#include <tinyxml2.h>

GameEngine::Behavior::For::For(const char *name) : 
	GameEngine::Behavior::Loop(name)
{
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
	node->m_start.set(xmlVar, node->m_tree);

	xmlVar = element->FirstChildElement("end");
	node->m_end.set(xmlVar, node->m_tree);

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
	int32_t& counter = m_tree->blackboard<int32_t>(m_variable);
	m_start.get(counter);
}

/**
 *
 */
void GameEngine::Behavior::For::execute(Action* r)
{
	int32_t& counter = m_tree->blackboard<int32_t>(m_variable);
	int32_t end;
	m_end.get(end);

	if (counter > end) {
		m_status = m_defaultreturn;
		return GameEngine::BehaviorNode::execute(r);
	}

	counter++;

	GameEngine::Behavior::Loop::execute(r);
}

/**
 * display the node data in the debugger
 */
void GameEngine::Behavior::For::debugGUInode(void)
{
	GameEngine::Behavior::Loop::debugGUInode();
}
