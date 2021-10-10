#include "dfFire2Player.h"

#include <imgui.h>

#include "../../darkforces/dfObject.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../config.h"

DarkForces::Behavior::Fire2Player::Fire2Player(const char* name):
	BehaviorNode(name)
{
}

/**
 * locate the player
 */
bool DarkForces::Behavior::Fire2Player::locatePlayer(void)
{
	m_position = m_player->position();

	// can we reach the player ?
	m_direction = glm::normalize(m_position - m_entity->position());

	// test line of sight from await of the entity to away from the player (to not catch the entity nor the player)
	glm::vec3 start = m_entity->position() + m_direction * (m_entity->radius() * 1.5f);
	start.y += m_entity->height() / 2.0f;
	glm::vec3 end = m_position - m_direction * (m_entity->radius() * 1.5f);
	end.y += m_player->height() / 2.0f;

	//start = glm::vec3(-24.09, 0.35, 29.44);
	//end = glm::vec3(-28.17, 0.16, 26.81);
	Framework::Segment segment(start, end);

	m_from = start;
	m_to = end;

	std::vector<gaEntity*> collisions;
	if (g_gaWorld.intersectWithEntity(segment, collisions)) {
		// check if there is a collision with something different than player and shooter
		bool real = false;
		for (auto entity : collisions) {
			if (entity != m_entity && entity != m_player) {
				real = true;
				break;
			}
		}

		if (real) {
			m_visibility = false;
			return false;
		}
	}

	m_visibility = true;

	// turn toward the player
	m_entity->sendMessage(gaMessage::LOOK_AT, -m_direction);

	// record last known position
	m_tree->blackboard("player_last_known_position", &m_position);

	return true;
}

/**
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::Fire2Player::init(void* data)
{
	m_status = Status::RUNNING;

	if (m_player == nullptr) {
		m_player = g_gaWorld.getEntity("player");
	}

	if (!locatePlayer()) {
		// can't reach the player, drop out
		m_status = Status::SUCCESSED;
	}
	else {
		m_state = 2;	// firing animation
		m_entity->sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::ENEMY_ATTACK);
	}
}

void DarkForces::Behavior::Fire2Player::dispatchMessage(gaMessage* message, Action* r)
{
	switch (message->m_action) {
	case DarkForces::Message::ANIM_START:
		if (m_state == 2) {
			m_firingFrames = message->m_value;
			m_firingFrame = 0;
		}
		break;

	case DarkForces::Message::ANIM_NEXT_FRAME:
		if (m_state == 2) {
			if (message->m_value == m_firingFrames-1) {
				// and fire
				m_entity->sendMessage(DarkForces::Message::FIRE, 0, (void*)&m_direction);
				m_state = 3;
			}
		}
		break;

	case DarkForces::Message::ANIM_END:
		if (m_state == 3) {
			// Fire animation ended, so reboot the move
			m_status = Status::SUCCESSED;
		}
		break;
	}
	BehaviorNode::execute(r);
}

/**
 * debugger
 */
void DarkForces::Behavior::Fire2Player::debugGUIinline(BehaviorNode* current)
{
	static char tmp[64];
	const char* p = m_name;

	if (this == current) {
		snprintf(tmp, sizeof(tmp), ">%s", m_name);
		p = tmp;
	}

	if (ImGui::TreeNode(p)) {
		ImGui::Text("visibility:%s", (m_visibility) ? "True" : "False");
		ImGui::Text("from:%.2f %.2f %.2f", m_from.x, m_from.y, m_from.z);
		ImGui::Text("to:%.2f %.2f %.2f", m_to.x, m_to.y, m_to.z);
		for (auto n : m_children)
			n->debugGUIinline(current);
		ImGui::TreePop();
	}
}

namespace DarkForces {
	namespace FlightRecorder {
		struct fire2player {
			struct GameEngine::FlightRecorder::BehaviorNode node;
			glm::vec3 from;
			glm::vec3 to;
			bool visibility;
		};
	}
}

/**
 * flight recorder
 */
uint32_t DarkForces::Behavior::Fire2Player::recordState(void* record)
{
	DarkForces::FlightRecorder::fire2player* r = static_cast<DarkForces::FlightRecorder::fire2player*>(record);

	GameEngine::BehaviorNode::recordState(&r->node);
	r->node.size = sizeof(DarkForces::FlightRecorder::fire2player);
	r->to = m_to;
	r->from = m_from;
	r->visibility = m_visibility;

	return r->node.size;
}

uint32_t DarkForces::Behavior::Fire2Player::loadState(void* record)
{
	DarkForces::FlightRecorder::fire2player* r = static_cast<DarkForces::FlightRecorder::fire2player*>(record);

	GameEngine::BehaviorNode::loadState(&r->node);
	m_to = r->to;
	m_from = r->from;
	m_visibility = r->visibility;

	return r->node.size;
}
