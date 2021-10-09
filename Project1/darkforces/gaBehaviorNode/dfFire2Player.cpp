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
void DarkForces::Behavior::Fire2Player::locatePlayer(void)
{
	// get the player position
	m_direction = glm::normalize(m_player->position() - m_entity->position());

	// turn toward the player
	m_entity->sendMessage(gaMessage::LOOK_AT, -m_direction);
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

	m_state = 2;	// firing animation
	locatePlayer();
	m_entity->sendMessage(DarkForces::Message::STATE, (uint32_t)dfState::ENEMY_ATTACK);
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