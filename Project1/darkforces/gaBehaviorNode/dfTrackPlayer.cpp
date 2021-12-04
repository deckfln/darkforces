#include "dfTrackPlayer.h"

#include <tinyxml2.h>

#include "../dfComponent/dfEnemyAI.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"

/**
 *
 */
void DarkForces::Behavior::TrackPlayer::onChildExit(uint32_t child, Status status)
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

BehaviorNode* DarkForces::Behavior::TrackPlayer::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	return new DarkForces::Behavior::TrackPlayer(name);
}

/**
 *
 */
void DarkForces::Behavior::TrackPlayer::init(void* data)
{
	// Pick the last 2 known positions and run the entity along the axe up to a wall
	std::vector<glm::vec3>* playerLastPositions = m_tree->blackboard<std::vector<glm::vec3>>("player_last_positions");
	uint32_t size = playerLastPositions->size();
	glm::vec2 direction;

	if (size < 2) {
		// if we don't have enough position of the player, were and when did we last heard a blaster shot

		glm::vec3* sound = m_tree->blackboard<glm::vec3>("last_heard_sound");
		if (sound == nullptr) {
			m_status = Status::FAILED;
			return;
		}

		m_target = glm::vec3(sound->x, m_entity->position().y, sound->z);
	}
	else {
		// the player may have been seen twice at the same position, so find a different position, but only go back a bit
		glm::vec3 p1 = playerLastPositions->at(--size);
		glm::vec3 p2 = playerLastPositions->at(--size);
		int i = 5;

		while (p1.x == p2.x && p1.z == p2.z && size > 0) {
			p2 = playerLastPositions->at(--size);
		}
		if (size <= 0) {
			m_status = Status::FAILED;
			return;
		}

		glm::vec2 p1d(p1.x, p1.z);
		glm::vec2 p2d(p2.x, p2.z);

#ifdef _DEBUG
		if (p1d == p2d) {
			__debugbreak();
		}
#endif

		direction = glm::normalize(p1d - p2d);
		m_target = glm::vec3(direction.x + p1d.x, m_entity->position().y, direction.y + p1d.y);
	}

	// walk only for 2s
	GameEngine::Alarm alarm(m_entity, 2000, gaMessage::Action::SatNav_CANCEL);
	m_alarmID = g_gaWorld.registerAlarmEvent(alarm);

	GameEngine::BehaviorNode::init(&m_target);
}

