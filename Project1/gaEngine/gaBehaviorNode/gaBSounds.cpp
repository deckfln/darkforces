#include "gaBSounds.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"

static const char* g_className = "GameEngine:Sounds";

GameEngine::Behavior::Sounds::Sounds(const char* name) :
	GameEngine::Behavior::SetVar(name)
{
	m_className = g_className;
}

GameEngine::BehaviorNode* GameEngine::Behavior::Sounds::clone(GameEngine::BehaviorNode* p)
{
	GameEngine::Behavior::Sounds* cl;
	if (p) {
		cl = dynamic_cast<GameEngine::Behavior::Sounds*>(p);
	}
	else {
		cl = new GameEngine::Behavior::Sounds(m_name);
	}
	GameEngine::Behavior::SetVar::clone(cl);
	return cl;
}

/**
 * generate from XML
 */
GameEngine::BehaviorNode* GameEngine::Behavior::Sounds::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::Sounds* node;

	if (used == nullptr) {
		node = new GameEngine::Behavior::Sounds(name);
	}
	else {
		node = dynamic_cast<GameEngine::Behavior::Sounds*>(used);
	}

	GameEngine::Behavior::SetVar::create(name, element, node);

	return node;
}

/**
 * set the variable with the top of the sounds lists
 */
void GameEngine::Behavior::Sounds::init(void* data)
{
	Var::init(nullptr);

	std::vector<Origin>& sounds = m_tree->blackboard().get<std::vector<Origin>>("sounds", GameEngine::Variable::Type::OBJECT);

	if (sounds.size() == 0) {
		m_status = GameEngine::BehaviorNode::Status::FAILED;
		return;
	}

	Origin& p = sounds.back();
	m_variable.set(m_tree, p.m_position);
	m_status = GameEngine::BehaviorNode::Status::SUCCESSED;
	sounds.pop_back();
}