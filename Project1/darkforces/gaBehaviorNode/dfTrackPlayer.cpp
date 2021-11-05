#include "dfTrackPlayer.h"

#include "../dfComponent/dfEnemyAI.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"

/**
 *
 */
void DarkForces::Behavior::TrackPlayer::onChildExit(Status status)
{
	// remove programmed alarm
	g_gaWorld.cancelAlarmEvent(m_alarmID);

	// check the player location, but only react to visibility, not to distance from last knwon position because we are away
	static_cast<DarkForces::Component::EnemyAI*>(m_tree)->viewPlayer();
	bool* b = m_tree->blackboard<bool>("player_visible");
	if (*b == false) {
		// we still can' see the player, so the tracking failed
		m_children[m_runningChild]->status(Status::FAILED);
	}
	else {
		m_children[m_runningChild]->status(Status::SUCCESSED);
	}
}

/**
 *
 */
DarkForces::Behavior::TrackPlayer::TrackPlayer(const char* name):
	GameEngine::Behavior::Decorator(name)
{
}

/**
 *
 */
void DarkForces::Behavior::TrackPlayer::init(void* data)
{
	// Pick the last 2 known positions and run the entity along the axe up to a wall
	std::vector<glm::vec3>* playerLastPositions = m_tree->blackboard<std::vector<glm::vec3>>("player_last_positions");
	uint32_t size = playerLastPositions->size();
	if (size < 2) {
		m_status = Status::FAILED;
		return;
	}

	// the player may have been seen twice at the same position, so find a different position
	glm::vec3 p1 = playerLastPositions->at(--size);
	glm::vec3 p2 = playerLastPositions->at(--size);
	
	while (p1 == p2 && size > 0) {
		p2 = playerLastPositions->at(--size);
	}
	if (size <= 0) {
		m_status = Status::FAILED;
		return;
	}

	glm::vec2 p1d(p1.x, p1.z);
	glm::vec2 p2d(p2.x, p2.z);

	glm::vec2 direction = glm::normalize(p1d - p2d);
	if (glm::length(direction) < m_entity->radius()) {
		m_status = Status::FAILED;
		return;
	}

	direction *= m_entity->radius() * 10.0f;

	glm::vec3 target(direction.x + m_entity->position().x, m_entity->position().y, direction.y + m_entity->position().z);

	m_navpoints.clear();
	m_navpoints.push_back(target);

	// walk only for 2s
	GameEngine::Alarm alarm(m_entity, 2000, gaMessage::Action::SatNav_CANCEL);
	m_alarmID = g_gaWorld.registerAlarmEvent(alarm);

	GameEngine::BehaviorNode::init(&m_navpoints);
}