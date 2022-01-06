#include "gaBSetVar.h"

#include <tinyxml2.h>
#include <imgui.h>
#include <map>

#include "../gaEntity.h"
#include "../World.h"
#include "../gaDebug.h"
#include "../gaComponent/gaBehaviorTree.h"

//-------------------------------------

static const char* g_className = "SetVar";

GameEngine::Behavior::SetVar::SetVar(const char* name) :
	Var(name)
{
	m_className = g_className;
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
	GameEngine::Behavior::Var::clone(cl);
	return cl;
}

void GameEngine::Behavior::SetVar::init(void*)
{
	switch (m_type) {
	case Type::BOOL:
		m_tree->blackboard().set<bool>(m_variable, m_value, GameEngine::Variable::Type::BOOL);
		break;
	case Type::INT32:
		m_tree->blackboard().set<int32_t>(m_variable, m_ivalue, GameEngine::Variable::Type::INT32);
		break;
	case Type::FLOAT:
		m_tree->blackboard().set<float>(m_variable, m_fvalue, GameEngine::Variable::Type::FLOAT);
		break;
	case Type::VEC3:
		m_tree->blackboard().set<glm::vec3>(m_variable, m_v3value, GameEngine::Variable::Type::VEC3);
		break;
	case Type::VAR:
		m_tree->blackboard().assign(m_variable, m_svalue);
		break;
	case Type::STRING:
		m_tree->blackboard().set<std::string>(m_variable, m_svalue, GameEngine::Variable::Type::STRING);
		break;
	}
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
	GameEngine::Behavior::Var::create(name, element, node);
	return node;
}
