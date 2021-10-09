#include "dfFire2Player.h"

#include "../../darkforces/dfObject.h"
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
	// can we reach the player ?
	m_direction = glm::normalize(m_player->position() - m_entity->position());

	// test line of sight from await of the entity to away from the player (to not catch the entity nor the player)
	glm::vec3 start = m_entity->position() + m_direction * (m_entity->radius() * 1.5f);
	start.y += m_entity->height() / 2.0f;
	glm::vec3 end = m_player->position() - m_direction * (m_entity->radius() * 1.5f);
	end.y += m_player->height() / 2.0f;
	Framework::Segment segment(start, end);

	gaEntity* entity = g_gaWorld.intersectWithEntity(segment);
	if (entity != nullptr && entity != m_entity && entity != m_player) {
		return false;
	}

	// turn toward the player
	m_entity->sendMessage(gaMessage::LOOK_AT, -m_direction);

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