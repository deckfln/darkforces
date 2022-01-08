#include "gaBCheckVar.h"

#include <tinyxml2.h>
#include <imgui.h>

#include "../gaEntity.h"
#include "../World.h"
#include "../gaDebug.h"
#include "../gaComponent/gaBehaviorTree.h"

//-------------------------------------

static const char* g_className = "CheckVar";

GameEngine::Behavior::CheckVar::CheckVar(const char* name) :
	Var(name)
{
	m_className = g_className;
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
	GameEngine::Behavior::Var::clone(cl);
	return cl;
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
	GameEngine::Behavior::Var::create(name, element, node);
	return node;
}

/**
 *
 */
void GameEngine::Behavior::CheckVar::init(void*)
{
	if (m_variable.equal(m_tree)) {
		m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
	}
	else {
		m_status = GameEngine::BehaviorNode::Status::FAILED;
	}
}
