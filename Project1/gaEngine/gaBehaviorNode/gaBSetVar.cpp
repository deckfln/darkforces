#include "gaBSetVar.h"

#include <tinyxml2.h>
#include <imgui.h>

#include "../gaEntity.h"
#include "../World.h"
#include "../gaDebug.h"
#include "../gaComponent/gaBehaviorTree.h"
#include "gaBSetVar.h"

//-------------------------------------

GameEngine::Behavior::SetVar::SetVar(const char* name) :
	BehaviorNode(name)
{
}

GameEngine::BehaviorNode* GameEngine::Behavior::SetVar::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::SetVar* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::SetVar*>(p);
	}
	else {
		cl = new GameEngine::Behavior::SetVar(m_name);
	}
	GameEngine::BehaviorNode::clone(cl);
	cl->m_variable = m_variable;
	cl->m_value = m_value;
	return cl;
}

void GameEngine::Behavior::SetVar::init(void*)
{
	m_tree->blackboard<bool>(m_variable, m_value);
	m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
}

/**
 *
 */
GameEngine::BehaviorNode* GameEngine::Behavior::SetVar::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::SetVar* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::SetVar(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::SetVar*>(used);
	}
	GameEngine::BehaviorNode::create(name, element, node);

	// get the variable name
	tinyxml2::XMLElement* xmlVariable = element->FirstChildElement("variable");
	const char* cname = xmlVariable->GetText();
	if (cname) {
		node->m_variable = cname;
		const char *type = xmlVariable->Attribute("type");
		if (strcmp(type, "bool") == 0) {
			bool b;
			if (xmlVariable->QueryBoolAttribute("value", &b) == tinyxml2::XML_SUCCESS) {
				node->m_value = b;
			}
		}
		else {
			gaDebugLog(1, "GameEngine::Behavior::SetVar", "unknown type");
		}
	}

	return node;
}

//----------------------------------------------

void GameEngine::Behavior::SetVar::debugGUInode(void)
{
	ImGui::Text("%s:%d", m_variable.c_str(), m_value);
}