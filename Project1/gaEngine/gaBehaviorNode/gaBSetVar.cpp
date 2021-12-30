#include "gaBSetVar.h"

#include <tinyxml2.h>
#include <imgui.h>
#include <map>

#include "../gaEntity.h"
#include "../World.h"
#include "../gaDebug.h"
#include "../gaComponent/gaBehaviorTree.h"

//-------------------------------------

GameEngine::Behavior::SetVar::SetVar(const char* name) :
	Var(name)
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
	GameEngine::Behavior::Var::clone(cl);
	return cl;
}

void GameEngine::Behavior::SetVar::init(void*)
{
	switch (m_type) {
	case Type::BOOL:
		m_tree->blackboard<bool>(m_variable, m_value);
		break;
	case Type::INT32:
		m_tree->blackboard<int32_t>(m_variable, m_ivalue);
		break;
	case Type::FLOAT:
		m_tree->blackboard<float>(m_variable, m_fvalue);
		break;
	case Type::VEC3:
		m_tree->blackboard<glm::vec3>(m_variable, m_v3value);
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
