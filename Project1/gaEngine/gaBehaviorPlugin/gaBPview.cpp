#include "gaBPview.h"

GameEngine::Behavior::Plugin::View::View(void) :
	Base("GameEngine:View")
{
}

/**
 * check if see the player in the cone of vision
 */
bool GameEngine::Behavior::Plugin::View::onViewPlayer(GameEngine::Blackboard& blackboard, gaMessage* message)
{
	std::deque<glm::vec3>& playerLastPositions = blackboard.get<std::deque<glm::vec3>>("player_last_positions", GameEngine::Variable::Type::OBJECT);

	// player is visible, because we just received a notification
	if (playerLastPositions.size() >= 32) {
		playerLastPositions.pop_front();
	}
	playerLastPositions.push_back(message->m_v3value);

	blackboard.set<bool>("player_visible", true, GameEngine::Variable::Type::BOOL);

	return true;
}

/**
 * player is viewed
 */
bool GameEngine::Behavior::Plugin::View::onNotViewPlayer(GameEngine::Blackboard& blackboard, gaMessage*)
{
	blackboard.set<bool>("player_visible", false, GameEngine::Variable::Type::BOOL);
	return true;
}

/** 
 *
 */
void GameEngine::Behavior::Plugin::View::dispatchMessage(GameEngine::Blackboard& blackboard, gaMessage* message)
{
	switch (message->m_value) {
	case gaMessage::VIEW:
		onViewPlayer(blackboard, message);
		break;
	case gaMessage::NOT_VIEW:
		onNotViewPlayer(blackboard, message);
		break;
	}
}

GameEngine::Behavior::Plugin::Base* GameEngine::Behavior::Plugin::View::Create(void)
{
	return new GameEngine::Behavior::Plugin::View();
}
