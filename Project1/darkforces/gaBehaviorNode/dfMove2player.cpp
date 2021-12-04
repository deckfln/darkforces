#include "dfMove2player.h"

#include <tinyxml2.h>
#include <imgui.h>

#include "../../darkforces/dfObject.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../gaEngine/gaNavMesh.h"
#include "../../config.h"

void DarkForces::Behavior::Move2Player::onChildExit(uint32_t child, Status status)
{
	// remove programmed alarm
	g_gaWorld.cancelAlarmEvent(m_alarmID);
}

DarkForces::Behavior::Move2Player::Move2Player(const char* name):
	GameEngine::Behavior::Decorator(name)
{
}

BehaviorNode* DarkForces::Behavior::Move2Player::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	return new DarkForces::Behavior::Move2Player(name);
}

/**
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::Move2Player::init(void* data)
{
	// this could be the real position (player is visible)
	// or the last known position (player is hidden)
	std::vector<glm::vec3>* playerLastPositions = m_tree->blackboard<std::vector<glm::vec3>>("player_last_positions");
	if (playerLastPositions->size() == 0) {
		m_status = Status::FAILED;
		return;
	}

	glm::vec3 move2 = playerLastPositions->back();

	// stop 8 clicks away from the player
	m_target = move2 - m_entity->position();
	float l = glm::length(m_target) - m_entity->radius() * 8.0f;
	if (l < 0) {
		m_status = Status::FAILED;
		return;
	}

	m_target = glm::normalize(m_target) * l;
	if (glm::length(m_target) < m_entity->radius()) {
		m_status = Status::FAILED;
		return;
	}
	m_target += m_entity->position();
	//m_target.y = m_entity->position().y;

	// walk only for 2s
	GameEngine::Alarm alarm(m_entity, 2000, gaMessage::Action::SatNav_CANCEL);
	m_alarmID = g_gaWorld.registerAlarmEvent(alarm);

	GameEngine::BehaviorNode::init(&m_target);
}

//---------------------------------------------------------------

/**
 * display the node data in the debugger
 */
void DarkForces::Behavior::Move2Player::debugGUInode(void)
{
	ImGui::Text("%.2f %.2f %.2f", m_target.x, m_target.y, m_target.z);
}
