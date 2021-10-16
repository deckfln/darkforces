#include "dfMoveToAndAttack.h"

#include <imgui.h>

#include "../../darkforces/dfObject.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../config.h"

DarkForces::Behavior::MoveToAndAttack::MoveToAndAttack(const char* name):
	GameEngine::Behavior::Loop(name)
{
}

static glm::vec3 debug[] = {
	{-24.52,0.01,31.75},
	{-24.50,0.01,30.02},
	{-24.44,0.01,28.97},
	{-27.24,-0.20,26.50},
	{-27.24,-0.20,26.50}
};
static uint32_t debug_i = 0;

/**
 * locate the player
 */
bool DarkForces::Behavior::MoveToAndAttack::locatePlayer(void)
{
	glm::vec3 position = debug[debug_i++];
	//glm::vec3 position = m_player->position();

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

	std::vector<glm::vec3>* playerLastPositions = m_tree->blackboard<std::vector<glm::vec3>>("player_last_positions");
	if (canSee) {
		playerLastPositions->push_back(position);
		m_tree->blackboard<bool>("player_visible", true);
	}
	else {
		m_tree->blackboard<bool>("player_visible", false);
		if (playerLastPositions->size() == 0) {
			// the player was never seen, so drop out
			m_status = Status::FAILED;
			return false;
		}

		// continue moving to the last known good position
		position = playerLastPositions->back();

		// if we are reaching the last known position and still can't see the player, give up
		float l = glm::distance(position, m_entity->position());
		if (l < m_entity->radius()) {
			m_status = Status::FAILED;
			return false;
		}
 	}

	// turn toward the player
	m_navpoints.clear();
	m_navpoints.push_back(position);

	return true;
}

/**
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::MoveToAndAttack::init(void* data)
{
	m_status = Status::RUNNING;
	m_runningChild = -1;

	if (m_player == nullptr) {
		m_player = g_gaWorld.getEntity("player");
	}
}

/**
 * let a parent take a decision with it's current running child
 */
void DarkForces::Behavior::MoveToAndAttack::execute(Action* r)
{
	locatePlayer();
	GameEngine::Behavior::Loop::execute(r);
}
