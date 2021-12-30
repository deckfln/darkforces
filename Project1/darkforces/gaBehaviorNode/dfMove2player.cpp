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
	GameEngine::Behavior::SetVar(name)
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
	GameEngine::Behavior::SetVar::clone(cl);
	return cl;
}

BehaviorNode* DarkForces::Behavior::Move2Player::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	DarkForces::Behavior::Move2Player* node = new DarkForces::Behavior::Move2Player(name);
	GameEngine::Behavior::SetVar::create(name, element, node);
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
		m_status = Status::FAILED;
		return;
	}

	glm::vec3 move2 = playerLastPositions->back();

	// stop 8 clicks away from the player
	m_target = move2 - m_entity->position();
	float l = glm::length(m_target) - m_entity->radius() * 8.0f;
	if (l < 0) {
		m_tree->blackboard<bool>("nearby_player", true);
		m_status = Status::SUCCESSED;
		return;
	}

	m_target = glm::normalize(m_target) * l;
	if (glm::length(m_target) < m_entity->radius()) {
		m_tree->blackboard<bool>("nearby_player", true);
		m_status = Status::SUCCESSED;
		return;
	}

	m_target += m_entity->position();
	//m_target.y = m_entity->position().y;

	m_v3value = m_target;
	m_tree->blackboard<bool>("nearby_player", false);

	GameEngine::Behavior::SetVar::init(&m_target);
}
