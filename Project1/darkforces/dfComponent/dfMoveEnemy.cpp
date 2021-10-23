#include "dfMoveEnemy.h"

#include <glm/vec3.hpp>

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"

DarkForces::Component::MoveEnemy::MoveEnemy():
	GameEngine::Component::BehaviorTree(&m_waitIdle)
{
	m_satnav.speed(1.0f);

	m_waitIdle.tree(this);
	m_waitIdle.init(nullptr);

	m_waitIdle.addNode(&m_attack);
		m_attack.addNode(&m_moveAndAttack);
			m_moveAndAttack.addNode(&m_move2player);
				m_move2player.addNode(&m_move2);
			m_moveAndAttack.addNode(&m_shootPlayer);
		m_attack.addNode(&m_track);
			m_track.addNode(&m_move3);

	m_waitIdle.addNode(&m_move_to);
		m_move_to.addNode(&m_satnav);
		m_move_to.addNode(&m_wait_door);
		m_move_to.addNode(&m_open_door);
			m_open_door.addNode(&m_goto_trigger);
				m_goto_trigger.addNode(&m_satnav_door);
			m_open_door.addNode(&m_wait_door_2);

	blackboard("player_last_positions", (void*)&m_playerLastPositions);
}

/**
 * Record player position
 */
void DarkForces::Component::MoveEnemy::dispatchMessage(gaMessage* message)
{
	if (message->m_frame > 0) {
		m_currentFrame = message->m_frame;
	}

	switch (message->m_action) {
	case gaMessage::Action::VIEW:
		m_lastPlayerView = message->m_v3value;
		m_lastPlayerViewFrame = message->m_frame;
		break;
	}

	GameEngine::Component::BehaviorTree::dispatchMessage(message);
}

/**
 * check if see the player in the cone of vision
 */
bool DarkForces::Component::MoveEnemy::viewPlayer(void)
{
	std::vector<glm::vec3>* playerLastPositions = blackboard<std::vector<glm::vec3>>("player_last_positions");

	if (m_currentFrame == m_lastPlayerViewFrame || m_currentFrame == m_lastPlayerViewFrame+1) {
		if (glm::distance(m_lastPlayerView, m_entity->position()) < m_entity->radius() * 8.0f) {
			// stay away of the target
			return true;
		}

		// player is visible, because we just received a notification
		playerLastPositions->push_back(m_lastPlayerView);
		blackboard<bool>("player_visible", true);

	}
	else {
		// player is not visible, the last time we saw it is in the past

		blackboard<bool>("player_visible", false);
		if (playerLastPositions->size() == 0) {
			// the player was never seen, so drop out
			return false;
		}

		// continue moving to the last known good position
		const glm::vec3& po = playerLastPositions->back();

		// if we are reaching the last known position and still can't see the player, give up
		float l = glm::distance(po, m_entity->position());
		if (l < m_entity->radius()) {
			return false;
		}
	}

	return true;
}

/**
 * locate the player fully around the player
 */
bool DarkForces::Component::MoveEnemy::locatePlayer(void)
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
