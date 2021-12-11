#include "dfEnemyAI.h"

#include <glm/vec3.hpp>

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"

#include "../dfMessage.h"
#include "../dfVOC.h"

DarkForces::Component::EnemyAI::EnemyAI():
	GameEngine::Component::BehaviorTree()
{
	blackboard("player_last_positions", (void*)&m_playerLastPositions);
	handlers(gaMessage::Action::VIEW, &GameEngine::Component::BehaviorTree::onViewPlayer);
	handlers(gaMessage::Action::NOT_VIEW, &GameEngine::Component::BehaviorTree::onNotViewPlayer);
	handlers(gaMessage::Action::HEAR_SOUND, &GameEngine::Component::BehaviorTree::onHearSound);
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
		// when the player starts dying, ignore any incoming messages
		m_discardMessages = true;
		break;

	}

	GameEngine::Component::BehaviorTree::dispatchMessage(message);
}

/**
 * locate the player fully around the player
 */
bool DarkForces::Component::EnemyAI::locatePlayer(void)
{
	if (m_player == nullptr) {
		m_player = g_gaWorld.getEntity("player");
	}

	glm::vec3 po = m_player->position();
	//printf("DarkForces::Behavior::MoveToAndAttack::locatePlayer::%.4f,%.4f;%.4f\n", position.x, position.y, position.z);

	// can we reach the player ?
	glm::vec3 direction = glm::normalize(po - m_entity->position());

	// test line of sight from await of the entity to away from the player (to not catch the entity nor the player)
	glm::vec3 start = m_entity->position() + direction * (m_entity->radius() * 1.5f);
	start.y += m_entity->height() / 2.0f;
	glm::vec3 end = po - direction * (m_entity->radius() * 1.5f);
	end.y += m_player->height() / 2.0f;
	Framework::Segment segment(start, end);

	if (segment.length() < m_entity->radius() * 8.0f) {
		// stay away of the target
		return true;
	}

	bool canSee = true;
	std::vector<gaEntity*> collisions;
	if (g_gaWorld.intersectWithEntity(segment, collisions)) {
		// check if there is a collision with something different than player and shooter
		for (auto entity : collisions) {
			if (entity != m_entity && entity != m_player) {
				canSee = false;
				break;
			}
		}
	}

	std::vector<glm::vec3>* playerLastPositions = blackboard<std::vector<glm::vec3>>("player_last_positions");
	if (canSee) {
		playerLastPositions->push_back(po);
		blackboard<bool>("player_visible", true);
	}
	else {
		blackboard<bool>("player_visible", false);
		if (playerLastPositions->size() == 0) {
			// the player was never seen, so drop out
			return false;
		}

		// continue moving to the last known good position
		po = playerLastPositions->back();

		// if we are reaching the last known position and still can't see the player, give up
		float l = glm::distance(po, m_entity->position());
		if (l < m_entity->radius()) {
			return false;
		}
	}

	return true;
}

/**
 *
 */
void DarkForces::Component::EnemyAI::addSound(const std::string& file, uint32_t id)
{
	//m_teasePlayer.addSound(loadVOC(file)->sound(), id);
}
