#include "gaBVar.h"

#include <tinyxml2.h>
#include <imgui.h>
#include <map>

#include "../gaEntity.h"
#include "../World.h"
#include "../gaDebug.h"
#include "../gaComponent/gaBehaviorTree.h"
#include "../gaValue.h"

//-------------------------------------

static const std::map<const char*, GameEngine::Value::Type> g_types1 = {
	{"bool", GameEngine::Value::Type::BOOL},
	{"int32", GameEngine::Value::Type::INT32},
	{"float", GameEngine::Value::Type::FLOAT},
	{"vec3", GameEngine::Value::Type::VEC3},
	{"string", GameEngine::Value::Type::STRING},
	{"var", GameEngine::Value::Type::VAR},
};

static const char* g_className = "Var";

/**
 * initialize variable from XML
 */
void GameEngine::Value::set(tinyxml2::XMLElement* xmlVar)
{
	const char* type = xmlVar->Attribute("type");
	for (auto& t : g_types1) {
		if (strcmp(type, t.first) == 0) {
			m_type = t.second;
			break;
		}
	}

	if (m_type == Type::NONE) {
		gaDebugLog(1, "GameEngine::Behavior::SetVar", "unknown type");
		exit(-1);
	}

	const char* v = xmlVar->Attribute("value");

	if (v) {
		if (v[0] == '=') {
			m_type = GameEngine::Value::Type::VAR;
			m_svalue = v + 1;
		}
		else {
			switch (m_type) {
			case Type::BOOL:
				xmlVar->QueryBoolAttribute("value", &m_value);
				break;
			case Type::INT32:
				xmlVar->QueryIntAttribute("value", &m_ivalue);
				break;
			case Type::FLOAT:
				xmlVar->QueryFloatAttribute("value", &m_fvalue);
				break;
			case Type::VEC3: {
				const char* v = xmlVar->Attribute("value");
				float x, y, z;
				if (v != nullptr) {
					scanf_s("%f,%f,%f", &x, &y, &z);
					m_v3value = glm::vec3(x, y, z);
				}
				// value defined by code, so not available here
				break; }
			case Type::STRING:
				m_svalue = xmlVar->Attribute("value");
				break;
			}
		}
	}
}

//-------------------------------------

GameEngine::Behavior::Var::Var(const char* name) :
	BehaviorNode(name)
{
	m_className = g_className;
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
	if (!node->m_variable.create(element)) {
		gaDebugLog(1, "GameEngine::Behavior::Var", "no variable defined for " + (std::string)node->m_name);
		exit(-1);
	}

	return node;
}

//----------------------------------------------

void GameEngine::Behavior::Var::debugGUInode(void)
{
	m_variable.debug();
}
