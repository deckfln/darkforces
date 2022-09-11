#include "gaBPHit.h"

#include "../gaBehaviorNode/gaBSounds.h"

GameEngine::Behavior::Plugin::Hit::Hit(void):
	Base("GameEngine:Hit")
{
}

/**
 * first message 'hear' a blaster, reset the list of sounds
 */
bool GameEngine::Behavior::Plugin::Hit::onHit(GameEngine::Blackboard& blackboard, gaMessage* message)
{
	blackboard.set<glm::vec3>("last_bullet", message->m_v3value, GameEngine::Variable::Type::VEC3);
	blackboard.set<bool>("hit_bullet", true, GameEngine::Variable::Type::BOOL);

	return true;
}

/**
 *
 */
void GameEngine::Behavior::Plugin::Hit::dispatchMessage(GameEngine::Blackboard& blackboard, gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::BULLET_HIT:
		onHit(blackboard, message);
		break;
	}
}

GameEngine::Behavior::Plugin::Base* GameEngine::Behavior::Plugin::Hit::Create(void)
{
	return new GameEngine::Behavior::Plugin::Hit();
}
