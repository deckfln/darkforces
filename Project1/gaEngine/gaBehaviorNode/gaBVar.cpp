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

static const std::map<const char*, GameEngine::Behavior::Var::Type> g_types = {
	{"bool", GameEngine::Behavior::Var::Type::BOOL},
	{"int32", GameEngine::Behavior::Var::Type::INT32},
	{"float", GameEngine::Behavior::Var::Type::FLOAT},
	{"vec3", GameEngine::Behavior::Var::Type::VEC3},
	{"string", GameEngine::Behavior::Var::Type::STRING},
	{"var", GameEngine::Behavior::Var::Type::VAR},
};

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
	case Type::VAR:
	case Type::STRING:
		m_svalue = xmlVar->Attribute("value");
		break;
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
	cl->m_type = m_type;
	cl->m_value = m_value;
	cl->m_ivalue = m_ivalue;
	cl->m_fvalue = m_fvalue;
	cl->m_v3value = m_v3value;
	cl->m_svalue = m_svalue;
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
		case Type::VEC3: {
			const char* v = xmlVariable->Attribute("value");
			float x, y, z;
			if (v != nullptr) {
				sscanf_s(v, "%f,%f,%f", &x, &y, &z);
				node->m_v3value = glm::vec3(x, y, z);
			}
			// value defined by code, so not available here
			break; }
		case Type::VAR:
		case Type::STRING:
			node->m_svalue = xmlVariable->Attribute("value");
			break;
		}
	}

	return node;
}

//----------------------------------------------

void GameEngine::Behavior::Var::debugGUInode(void)
{
	switch (m_type) {
	case Type::BOOL:
		ImGui::Text("%s:%d", m_variable.c_str(), m_value);
		break;
	case Type::INT32:
		ImGui::Text("%s:%d", m_variable.c_str(), m_ivalue);
		break;
	case Type::FLOAT:
		ImGui::Text("%s:%.2f", m_variable.c_str(), m_fvalue);
		break;
	case Type::VEC3: {
		ImGui::Text("%s:%.2f %.2f %.2f", m_variable.c_str(), m_v3value.x, m_v3value.y, m_v3value.z);
		break; }
	case Type::VAR:
	case Type::STRING:
		ImGui::Text("%s:%s", m_variable.c_str(), m_svalue.c_str());
		break;
	}
}
