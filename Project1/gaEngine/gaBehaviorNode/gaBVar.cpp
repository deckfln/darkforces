#include "gaBVar.h"

#include <tinyxml2.h>
#include <imgui.h>
#include <map>

#include "../gaEntity.h"
#include "../World.h"
#include "../gaDebug.h"
#include "../gaComponent/gaBehaviorTree.h"

static const std::map<const char*, GameEngine::Behavior::Var::Type> g_types = {
	{"bool", GameEngine::Behavior::Var::Type::BOOL},
	{"uint32", GameEngine::Behavior::Var::Type::INT32},
	{"float", GameEngine::Behavior::Var::Type::FLOAT},
	{"vec3", GameEngine::Behavior::Var::Type::VEC3},
};

//-------------------------------------

GameEngine::Behavior::Var::Var(const char* name) :
	BehaviorNode(name)
{
}

GameEngine::BehaviorNode* GameEngine::Behavior::Var::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::Var* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::Var*>(p);
	}
	else {
		cl = new GameEngine::Behavior::Var(m_name);
	}
	GameEngine::BehaviorNode::clone(cl);
	cl->m_variable = m_variable;
	cl->m_type = m_type;
	cl->m_value = m_value;
	cl->m_ivalue = m_ivalue;
	cl->m_fvalue = m_fvalue;
	cl->m_v3value = cl->m_v3value;
	return cl;
}

/**
 *
 */
GameEngine::BehaviorNode* GameEngine::Behavior::Var::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::Var* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::Var(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::Var*>(used);
	}
	GameEngine::BehaviorNode::create(name, element, node);

	// get the variable name
	tinyxml2::XMLElement* xmlVariable = element->FirstChildElement("variable");
	const char* cname = xmlVariable->GetText();
	if (cname) {
		node->m_variable = cname;
		const char *type = xmlVariable->Attribute("type");
		for (auto& t : g_types) {
			if (strcmp(type, t.first) == 0) {
				node->m_type = t.second;
				break;
			}
		}

		if (node->m_type == Type::NONE) {
			gaDebugLog(1, "GameEngine::Behavior::SetVar", "unknown type");
			exit(-1);
		}

		switch (node->m_type) {
		case Type::BOOL:
			xmlVariable->QueryBoolAttribute("value", &node->m_value);
			break;
		case Type::INT32:
			xmlVariable->QueryIntAttribute("value", &node->m_ivalue);
			break;
		case Type::FLOAT:
			xmlVariable->QueryFloatAttribute("value", &node->m_fvalue);
			break;
		case Type::VEC3:
			// value defined by code, so not available here
			break;
		}
	}

	return node;
}

//----------------------------------------------

void GameEngine::Behavior::Var::debugGUInode(void)
{
	ImGui::Text("%s:%d", m_variable.c_str(), m_value);
}