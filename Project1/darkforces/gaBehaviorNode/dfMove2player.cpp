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
	glm::vec3 position = m_player->position();

	// can we reach the player ?
	glm::vec3 direction = glm::normalize(position - m_entity->position());

	// test line of sight from await of the entity to away from the player (to not catch the entity nor the player)
	glm::vec3 start = m_entity->position() + direction * (m_entity->radius() * 1.5f);
	start.y += m_entity->height() / 2.0f;
	glm::vec3 end = position - direction * (m_entity->radius() * 1.5f);
	end.y += m_player->height() / 2.0f;
	Framework::Segment segment(start, end);

	if (segment.length() < m_entity->radius() * 8.0f) {
		// stay away of the target
		m_status = Status::SUCCESSED;
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

	if (canSee) {
		m_lastKnwonPosition = position;
		m_tree->blackboard<glm::vec3>("player_last_known_position", m_lastKnwonPosition);
	}
	else {
		// continue moving to the last known good position
		position = m_lastKnwonPosition;

		// if we are reaching the last known position and still can't see the player, give up
		float l = glm::distance(position, m_entity->position());
		if (l < m_entity->radius()) {
			return false;
		}
 	}

	// turn toward the player
	m_navpoints.clear();
	m_navpoints.push_back(position);

	// walk for 2s
	GameEngine::Alarm alarm(m_entity, 2000, gaMessage::Action::SatNav_CANCEL);
	m_alarmID = g_gaWorld.registerAlarmEvent(alarm);
	return true;
}

/**
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::Move2Player::init(void* data)
{
	m_status = Status::RUNNING;
	m_runningChild = -1;

	if (m_player == nullptr) {
		m_player = g_gaWorld.getEntity("player");
	}

	// broadcast the beginning of the move (for animation)
	m_entity->sendMessage(gaMessage::START_MOVE);		// start the entity animation

	if (!locatePlayer()) {
		// drop out if can't see the player
		m_status = Status::FAILED;
		m_tree->blackboard("player_last_known_position", nullptr);
		return;
	}
}

/**
 * let a parent take a decision with it's current running child
 */
void DarkForces::Behavior::Move2Player::execute(Action* r)
{
	if (m_status != Status::RUNNING) {
		return BehaviorNode::execute(r);
	}

	if (m_runningChild < 0) {
		m_runningChild = 0;
		return startChild(r, m_runningChild, &m_navpoints);
	}

	switch (m_children[m_runningChild]->status()) {
	case Status::SUCCESSED:
	case Status::FAILED:
		// remove programmed alarm
		g_gaWorld.cancelAlarmEvent(m_alarmID);

		// drop out of the loop
		m_status = m_children[m_runningChild]->status();
		r->action = BehaviorNode::Status::EXIT;
		r->status = m_status;
		break;

	default:
		r->action = BehaviorNode::Status::RUNNING;
		break;
	}
}

