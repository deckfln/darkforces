#include "dfAttackPlayer.h"

#include <imgui.h>
#include <tinyxml2.h>

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../config.h"

DarkForces::Behavior::AttackPlayer::AttackPlayer(const char* name):
	GameEngine::Behavior::Loop(name)
{
}

BehaviorNode* DarkForces::Behavior::AttackPlayer::clone(GameEngine::BehaviorNode* p)
{
	DarkForces::Behavior::AttackPlayer* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::AttackPlayer*>(p);
	}
	else {
		cl = new DarkForces::Behavior::AttackPlayer(m_name);
	}
	GameEngine::Behavior::Loop::clone(cl);
	return cl;
}

BehaviorNode* DarkForces::Behavior::AttackPlayer::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	DarkForces::Behavior::AttackPlayer* node;

	if (used == nullptr) {
		node = new DarkForces::Behavior::AttackPlayer(name);
	}
	else {
		node = dynamic_cast<DarkForces::Behavior::AttackPlayer*>(used);
	}
	GameEngine::Behavior::Loop::create(name, element, node);

	return node;
}

/**
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::AttackPlayer::init(void* data)
{
	if (m_player == nullptr) {
		m_player = g_gaWorld.getEntity("player");
	}

	GameEngine::Behavior::Loop::init(data);
}

namespace DarkForces {
	namespace FlightRecorder {
		struct AttackPlayer {
			struct GameEngine::FlightRecorder::BehaviorNode node;
			uint32_t nbPositions;
			glm::vec3 positions[1];
		};
	}
}

/**
 * flight recorder
 */
uint32_t DarkForces::Behavior::AttackPlayer::recordState(void* record)
{
	DarkForces::FlightRecorder::AttackPlayer* r = static_cast<DarkForces::FlightRecorder::AttackPlayer*>(record);

	GameEngine::BehaviorNode::recordState(&r->node);
	std::deque<glm::vec3>& playerLastPositions = m_tree->blackboard<std::deque<glm::vec3>>("player_last_positions");
	r->node.size = sizeof(DarkForces::FlightRecorder::AttackPlayer) + sizeof(glm::vec3) * playerLastPositions.size();

	if (playerLastPositions.size() == 0) {
		r->nbPositions = 0;
	}
	else {
		r->nbPositions = playerLastPositions.size();
		for (uint32_t i = 0; i < r->nbPositions; i++) {
			r->positions[i] = playerLastPositions[i];
		}
	}

	return r->node.size;
}

uint32_t DarkForces::Behavior::AttackPlayer::loadState(void* record)
{
	DarkForces::FlightRecorder::AttackPlayer* r = static_cast<DarkForces::FlightRecorder::AttackPlayer*>(record);

	GameEngine::BehaviorNode::loadState(&r->node);
	std::deque<glm::vec3>& playerLastPositions = m_tree->blackboard<std::deque<glm::vec3>>("player_last_positions");
	playerLastPositions.clear();
	if (r->nbPositions != 0) {
		for (uint32_t i = 0; i < r->nbPositions; i++) {
			playerLastPositions.push_back(r->positions[i]);
		}
	}

	return r->node.size;
}
