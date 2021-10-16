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
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::Move2Player::init(void* data)
{
	m_status = Status::RUNNING;
	m_runningChild = -1;

	// this could be the real position (player is visible)
	// or the last known position (player is hidden)
	std::vector<glm::vec3>* playerLastPositions = m_tree->blackboard<std::vector<glm::vec3>>("player_last_positions");
	glm::vec3 move2 = playerLastPositions->back();

	if (glm::distance(move2, m_entity->position()) < m_entity->radius()) {
		m_status = Status::FAILED;
		return;
	}

	m_navpoints.clear();
	m_navpoints.push_back(playerLastPositions->back());

	// broadcast the beginning of the move (for animation)
	m_entity->sendMessage(gaMessage::START_MOVE);		// start the entity animation

	// walk only for 2s
	GameEngine::Alarm alarm(m_entity, 2000, gaMessage::Action::SatNav_CANCEL);
	m_alarmID = g_gaWorld.registerAlarmEvent(alarm);
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

