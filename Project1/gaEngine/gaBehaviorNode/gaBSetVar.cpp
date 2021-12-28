#include "gaBSetVar.h"

#include <tinyxml2.h>
#include <imgui.h>

#include "../gaEntity.h"
#include "../World.h"
#include "../gaDebug.h"
#include "../gaComponent/gaBehaviorTree.h"

//-------------------------------------

GameEngine::Behavior::SetVar::SetVar(const char* name) :
	BehaviorNode(name)
{
}

void GameEngine::Behavior::SetVar::init(void*)
{
	m_tree->blackboard<bool>(m_name, m_value);
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

	// get the variable name
	tinyxml2::XMLElement* xmlVariable = element->FirstChildElement("variable");
	const char* cname = xmlVariable->GetText();
	if (cname) {
		node->m_name = cname;
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
	ImGui::Text("%s:%d", m_name.c_str(), m_value);
}