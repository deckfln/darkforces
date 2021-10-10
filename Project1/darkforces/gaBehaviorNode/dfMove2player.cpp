#include "dfMove2player.h"

#include "../../darkforces/dfObject.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../config.h"

DarkForces::Behavior::Move2Player::Move2Player(const char* name):
	BehaviorNode(name)
{
}

/**
 * locate the player
 */
bool DarkForces::Behavior::Move2Player::locatePlayer(void)
{
	m_position = m_player->position();

	// can we reach the player ?
	glm::vec3 direction = glm::normalize(m_position - m_entity->position());

	// test line of sight from await of the entity to away from the player (to not catch the entity nor the player)
	glm::vec3 start = m_entity->position() + direction * (m_entity->radius() * 1.5f);
	start.y += m_entity->height() / 2.0f;
	glm::vec3 end = m_position - direction * (m_entity->radius() * 1.5f);
	end.y += m_player->height() / 2.0f;
	Framework::Segment segment(start, end);

	std::vector<gaEntity*> collisions;
	if (g_gaWorld.intersectWithEntity(segment, collisions)) {
		// check if there is a collision with something different than player and shooter
		bool real = false;
		for (auto entity : collisions) {
			if (entity != m_entity && entity != m_player) {
				real = true;
				break;
			}
		}

		if (real) {
			// continue moving to the last known good position
			glm::vec3* lastKnown = static_cast<glm::vec3*>(m_tree->blackboard("player_last_known_position"));
			if (lastKnown == nullptr) {
				return false;
			}

			m_position = *lastKnown;

			// if we are reaching the last known position and still can't see the player, give up
			float l = glm::distance(m_position, m_entity->position());
			if (l < m_entity->radius()) {
				return false;
			}
			return true;
		}
	}

	// turn toward the player
	m_direction = direction;
	m_entity->sendMessage(gaMessage::LOOK_AT, -m_direction);

	m_tree->blackboard("player_last_known_position", &m_position);

	// walk for 2s
	GameEngine::Alarm alarm;
	alarm.m_entity = m_entity;
	alarm.m_delay = 2000;

	g_gaWorld.registerAlarmEvent(alarm);
	return true;
}

/**
 *
 */
void DarkForces::Behavior::Move2Player::triggerMove(void)
{
	// and start moving toward the player
	m_entity->pTransform()->m_position = m_entity->position() + m_direction*0.01f;
	m_entity->sendDelayedMessage(
		gaMessage::WANT_TO_MOVE,
		gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL,
		m_entity->pTransform());
}

/**
 * manage move actions
 */
void DarkForces::Behavior::Move2Player::onMove(gaMessage* message, Action* r)
{
	triggerMove();
}

/**
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::Move2Player::init(void* data)
{
	m_status = Status::RUNNING;

	if (m_player == nullptr) {
		m_player = g_gaWorld.getEntity("player");
	}

	m_steps = -1;

	// broadcast the beginning of the move (for animation)
	m_entity->sendMessage(gaMessage::START_MOVE);		// start the entity animation

	if (!locatePlayer()) {
		// drop out if can't see the player
		m_status = Status::FAILED;
		m_tree->blackboard("player_last_known_position", nullptr);
		return;
	}

	m_steps = 30;
	triggerMove();
}

void DarkForces::Behavior::Move2Player::dispatchMessage(gaMessage* message, Action* r)
{
	switch (message->m_action) {
	case gaMessage::Action::MOVE:
		onMove(message, r);
		break;

	case gaMessage::Action::ALARM:
		m_status = Status::SUCCESSED;
		break;

	case gaMessage::Action::COLLIDE:
		m_entity->sendMessage(gaMessage::Action::END_MOVE);	// stop the entity animation
		return failed(r);
		break;
	}
	BehaviorNode::execute(r);
}