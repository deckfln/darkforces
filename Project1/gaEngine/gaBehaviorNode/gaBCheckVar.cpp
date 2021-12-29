#include "gaBCheckVar.h"

#include <tinyxml2.h>
#include <imgui.h>

#include "../gaEntity.h"
#include "../World.h"
#include "../gaDebug.h"
#include "../gaComponent/gaBehaviorTree.h"

//-------------------------------------

GameEngine::Behavior::CheckVar::CheckVar(const char* name) :
	BehaviorNode(name)
{
}

BehaviorNode* GameEngine::Behavior::CheckVar::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::CheckVar* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::CheckVar*>(p);
	}
	else {
		cl = new GameEngine::Behavior::CheckVar(m_name);
	}
	cl->m_variable = m_variable;
	cl->m_value = m_value;
	return cl;
}

void GameEngine::Behavior::CheckVar::init(void*)
{
	bool* b;
	b = m_tree->blackboard<bool>(m_variable);
	if (b && *b == m_value) {
		m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
	}
	else {
		m_status = GameEngine::BehaviorNode::Status::FAILED;
	}
}

/**
 *
 */
GameEngine::BehaviorNode* GameEngine::Behavior::CheckVar::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::CheckVar* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::CheckVar(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::CheckVar*>(used);
	}

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

void GameEngine::Behavior::CheckVar::debugGUInode(void)
{
	ImGui::Text("%s:%d", m_variable.c_str(), m_value);
}