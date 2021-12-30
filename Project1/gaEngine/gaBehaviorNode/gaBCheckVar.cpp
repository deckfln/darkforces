#include "gaBCheckVar.h"

#include <tinyxml2.h>
#include <imgui.h>

#include "../gaEntity.h"
#include "../World.h"
#include "../gaDebug.h"
#include "../gaComponent/gaBehaviorTree.h"

//-------------------------------------

GameEngine::Behavior::CheckVar::CheckVar(const char* name) :
	Var(name)
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
	switch (m_type) {
	case Type::BOOL: {
		bool* b;
		b = m_tree->blackboard<bool>(m_variable);
		if (b && *b == m_value) {
			m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
		}
		else {
			m_status = GameEngine::BehaviorNode::Status::FAILED;
		}
		break; }

	case Type::INT32: {
		int32_t* b;
		b = m_tree->blackboard<int32_t>(m_variable);
		if (b && *b == m_ivalue) {
			m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
		}
		else {
			m_status = GameEngine::BehaviorNode::Status::FAILED;
		}
		break; }

	case Type::FLOAT: {
		float* b;
		b = m_tree->blackboard<float>(m_variable);
		if (b && *b == m_fvalue) {
			m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
		}
		else {
			m_status = GameEngine::BehaviorNode::Status::FAILED;
		}
		break; }

	case Type::VEC3: {
		glm::vec3* b;
		b = m_tree->blackboard<glm::vec3>(m_variable);
		if (b && *b == m_v3value) {
			m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
		}
		else {
			m_status = GameEngine::BehaviorNode::Status::FAILED;
		}
		break; }
	}
}
