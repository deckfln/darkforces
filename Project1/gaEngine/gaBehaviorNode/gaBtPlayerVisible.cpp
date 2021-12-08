#include "gaBtPlayerVisible.h"

#include <imgui.h>
#include <tinyxml2.h>
#include <map>

#include "../gaComponent/gaBehaviorTree.h"

 /**
  * Create a node
  */
GameEngine::Behavior::PlayerVisible::PlayerVisible(const char* name) :
	BehaviorNode(name)
{
}

GameEngine::BehaviorNode* GameEngine::Behavior::PlayerVisible::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	GameEngine::Behavior::PlayerVisible* node = new GameEngine::Behavior::PlayerVisible(name);
	return node;
}

/**
 *
 */
void GameEngine::Behavior::PlayerVisible::execute(Action* r)
{
	bool* visible = m_tree->blackboard<bool>("player_visible");
	if (visible==nullptr || *visible == false) {
		return failed(r);
	}

	return succeeded(r);
}
