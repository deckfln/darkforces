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

/**
 *
 */
void GameEngine::Behavior::SetVar::init(void*)
{
	//m_variable.set(m_tree);
	m_variable.set(m_tree, m_value);

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
