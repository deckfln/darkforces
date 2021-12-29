#include "dfMove2player.h"

#include <tinyxml2.h>
#include <imgui.h>

#include "../../darkforces/dfObject.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../gaEngine/gaNavMesh.h"
#include "../../config.h"

DarkForces::Behavior::Move2Player::Move2Player(const char* name):
	GameEngine::Behavior::Decorator(name)
{
}

GameEngine::BehaviorNode* DarkForces::Behavior::Move2Player::clone(GameEngine::BehaviorNode* p)
{
	DarkForces::Behavior::Move2Player* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::Move2Player*>(p);
	}
	else {
		cl = new DarkForces::Behavior::Move2Player(m_name);
	}
	GameEngine::Behavior::Decorator::clone(cl);
	return cl;
}

BehaviorNode* DarkForces::Behavior::Move2Player::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	DarkForces::Behavior::Move2Player* node = new DarkForces::Behavior::Move2Player(name);
	GameEngine::Behavior::Decorator::create(name, element, node);
	return node;
}

/**
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::Move2Player::init(void* data)
{
	// this could be the real position (player is visible)
	// or the last known position (player is hidden)
	std::deque<glm::vec3>* playerLastPositions = m_tree->blackboard<std::deque<glm::vec3>>("player_last_positions");
	if (playerLastPositions->size() == 0) {
		glm::vec3* last_sound = m_tree->blackboard<glm::vec3>("last_heard_position");
		if (last_sound) {
			m_target = *last_sound;
			m_tree->blackboard<glm::vec3>("last_seen_heard", &m_target);
			m_status = Status::SUCCESSED;
			return;
		}
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

	m_tree->blackboard<glm::vec3>("last_seen_heard", &m_target);

	GameEngine::BehaviorNode::init(&m_target);
}

//---------------------------------------------------------------

/**
 * display the node data in the debugger
 */
void DarkForces::Behavior::Move2Player::debugGUInode(void)
{
	ImGui::Text("target:%.2f %.2f %.2f", m_target.x, m_target.y, m_target.z);
}
