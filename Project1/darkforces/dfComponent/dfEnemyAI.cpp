#include "dfEnemyAI.h"

#include <glm/vec3.hpp>

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"

#include "../dfMessage.h"
#include "../dfVOC.h"

DarkForces::Component::EnemyAI::EnemyAI():
	GameEngine::Component::BehaviorTree()
{
}

/**
 * Record player position
 */
void DarkForces::Component::EnemyAI::dispatchMessage(gaMessage* message)
{
	if (m_discardMessages) {
		// player is dying, ignore messages
		return;
	}

	if (message->m_frame > 0) {
		m_currentFrame = message->m_frame;
	}

	switch (message->m_action) {
	case DarkForces::Message::DYING:
		//onDying(message);
		break;

	}

	GameEngine::Component::BehaviorTree::dispatchMessage(message);
}

/**
 *
 */
bool DarkForces::Component::EnemyAI::onDying(gaMessage*)
{
	// when the player starts dying, ignore any incoming messages
	m_discardMessages = true;
	return true;
}

/**
 *
 */
void DarkForces::Component::EnemyAI::addSound(const std::string& file, uint32_t id)
{
	//m_teasePlayer.addSound(loadVOC(file)->sound(), id);
}
