#include "dfTrackPlayer.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"

DarkForces::Behavior::TrackPlayer::TrackPlayer(const char* name):
	GameEngine::BehaviorNode(name)
{
}

void DarkForces::Behavior::TrackPlayer::init(void* data)
{
	// Pick the last 2 known positions and run the entity along the axe up to a wall
	std::vector<glm::vec3>* playerLastPositions = m_tree->blackboard<std::vector<glm::vec3>>("player_last_positions");

	uint32_t size = playerLastPositions->size();
	if (size < 2) {
		m_status = Status::FAILED;
		return;
	}

	glm::vec3 p1 = playerLastPositions->at(size - 1);
	glm::vec3 p2 = playerLastPositions->at(size - 2);

	glm::vec3 direction = glm::normalize(p1 - p2);
	if (glm::length(direction) < m_entity->radius()) {
		m_status = Status::FAILED;
		return;
	}

	direction *= m_entity->radius() * 10.0f;

	m_navpoints.clear();
	m_navpoints.push_back(direction);

	// walk only for 2s
	GameEngine::Alarm alarm(m_entity, 2000, gaMessage::Action::SatNav_CANCEL);
	m_alarmID = g_gaWorld.registerAlarmEvent(alarm);

	GameEngine::BehaviorNode::init(data);
}

void DarkForces::Behavior::TrackPlayer::execute(Action* r)
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