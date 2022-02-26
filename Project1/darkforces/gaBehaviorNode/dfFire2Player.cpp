#include "dfFire2Player.h"

#include <imgui.h>
#include <tinyxml2.h>

#include "../../darkforces/dfObject.h"

#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../config.h"

static const char* g_className = "DarkForces:fire2player";

void DarkForces::Behavior::Fire2Player::fireNow(void)
{
	// and fire
	std::deque<glm::vec3>& playerLastPositions = m_tree->blackboard().get<std::deque<glm::vec3>>("player_last_positions", GameEngine::Variable::Type::OBJECT);
	const glm::vec3& p = playerLastPositions.back();
	m_direction = glm::normalize(p - m_entity->position());

	//printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", p.x, p.z, m_entity->position().x, m_entity->position().z, m_direction.x, m_direction.z);

	m_entity->sendMessage(DarkForces::Message::START_FIRE, 0, (void*)&m_direction);
}

DarkForces::Behavior::Fire2Player::Fire2Player(const char* name):
	BehaviorNode(name)
{
	m_className = g_className;
}

GameEngine::BehaviorNode* DarkForces::Behavior::Fire2Player::clone(GameEngine::BehaviorNode* p)
{
	DarkForces::Behavior::Fire2Player* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::Fire2Player*>(p);
	}
	else {
		cl = new DarkForces::Behavior::Fire2Player(m_name);
	}
	return cl;
}

BehaviorNode* DarkForces::Behavior::Fire2Player::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	return new DarkForces::Behavior::Fire2Player(name);
}


/**
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::Fire2Player::init(void* data)
{
	m_entity->sendMessage(DarkForces::Message::FORCE_STATE, (uint32_t)dfState::ENEMY_ATTACK, 1.0f);

	GameEngine::BehaviorNode::init(data);
}

/**
 *
 */
void DarkForces::Behavior::Fire2Player::dispatchMessage(gaMessage* message, Action* r)
{
	switch (message->m_action) {
	case DarkForces::Message::ANIM_START:
		if (message->m_value == (uint32_t)dfState::ENEMY_ATTACK) {
			m_firingFrames = *(uint32_t*)message->m_extra;

			// if the attack_enemy state has only 1 frame, fire immediately
			if (m_firingFrames == 1) {
				fireNow();
			}
		}
		break;

	case DarkForces::Message::ANIM_NEXT_FRAME:
		if (message->m_value == (uint32_t)dfState::ENEMY_ATTACK) {
			if (message->m_value == m_firingFrames - 1) {
				fireNow();
			}
		}
		break;

	case DarkForces::Message::ANIM_END:
		if (message->m_value == (uint32_t)dfState::ENEMY_ATTACK) {
			// Fire animation ended, so reboot the move
			m_entity->sendMessage(DarkForces::Message::STOP_FIRE);
			m_status = Status::SUCCESSED;
		}
		break;
	}
	BehaviorNode::execute(r);
}

/**
 * debugger
 */
void DarkForces::Behavior::Fire2Player::debugGUInode(GameEngine::Component::BehaviorTree* tree)
{
	ImGui::Text("from:%.2f %.2f %.2f", m_from.x, m_from.y, m_from.z);
	ImGui::Text("to:%.2f %.2f %.2f", m_to.x, m_to.y, m_to.z);
}

namespace DarkForces {
	namespace FlightRecorder {
		struct fire2player {
			struct GameEngine::FlightRecorder::BehaviorNode node;
			glm::vec3 from;
			glm::vec3 to;
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

	return r->node.size;
}

uint32_t DarkForces::Behavior::Fire2Player::loadState(void* record)
{
	DarkForces::FlightRecorder::fire2player* r = static_cast<DarkForces::FlightRecorder::fire2player*>(record);

	GameEngine::BehaviorNode::loadState(&r->node);
	m_to = r->to;
	m_from = r->from;

	return r->node.size;
}
