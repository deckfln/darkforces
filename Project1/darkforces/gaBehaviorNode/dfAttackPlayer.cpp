#include "dfAttackPlayer.h"

#include <imgui.h>

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../config.h"

DarkForces::Behavior::AttackPlayer::AttackPlayer(const char* name):
	GameEngine::Behavior::Loop(name)
{
}

/**
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::AttackPlayer::init(void* data)
{
	if (m_player == nullptr) {
		m_player = g_gaWorld.getEntity("player");
	}

	m_playerLastPositions.clear();	// reset the list of positions
	m_tree->blackboard("player_last_positions", (void*)&m_playerLastPositions);

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
 * debugger
 */
void DarkForces::Behavior::AttackPlayer::debugGUInode(void)
{
	if (ImGui::TreeNode("player last positions")) {
		for (auto& p : m_playerLastPositions) {
			ImGui::Text("%.2f %.2f %.2f", p.x, p.y, p.z);
		}
		ImGui::TreePop();
	}
}

/**
 * flight recorder
 */
uint32_t DarkForces::Behavior::AttackPlayer::recordState(void* record)
{
	DarkForces::FlightRecorder::AttackPlayer* r = static_cast<DarkForces::FlightRecorder::AttackPlayer*>(record);

	GameEngine::BehaviorNode::recordState(&r->node);
	r->node.size = sizeof(DarkForces::FlightRecorder::AttackPlayer) + sizeof(glm::vec3) * m_playerLastPositions.size();

	if (m_playerLastPositions.size() == 0) {
		r->nbPositions = 0;
	}
	else {
		r->nbPositions = m_playerLastPositions.size();
		for (uint32_t i = 0; i < r->nbPositions; i++) {
			r->positions[i] = m_playerLastPositions[i];
		}
	}

	return r->node.size;
}

uint32_t DarkForces::Behavior::AttackPlayer::loadState(void* record)
{
	DarkForces::FlightRecorder::AttackPlayer* r = static_cast<DarkForces::FlightRecorder::AttackPlayer*>(record);

	GameEngine::BehaviorNode::loadState(&r->node);
	m_playerLastPositions.clear();
	if (r->nbPositions != 0) {
		for (uint32_t i = 0; i < r->nbPositions; i++) {
			m_playerLastPositions.push_back(r->positions[i]);
		}
	}

	return r->node.size;
}
