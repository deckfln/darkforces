#include "dfMove2player.h"

#include "../../darkforces/dfObject.h"
#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../config.h"

DarkForces::Behavior::Move2Player::Move2Player(const char* name):
	BehaviorNode(name)
{
}

/**
 * locate the player
 */
void DarkForces::Behavior::Move2Player::locatePlayer(void)
{
	// get the player position
	m_direction = glm::normalize(m_player->position() - m_entity->position());

	// turn toward the player
	m_entity->sendMessage(gaMessage::LOOK_AT, -m_direction);
}

/**
 *
 */
void DarkForces::Behavior::Move2Player::triggerMove(void)
{
	// and start moving toward the player
	m_entity->pTransform()->m_position = m_entity->position() + m_direction*0.01f;
	m_entity->sendDelayedMessage(
		gaMessage::WANT_TO_MOVE,
		gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL,
		m_entity->pTransform());
}

/**
 * manage move actions
 */
void DarkForces::Behavior::Move2Player::onMove(gaMessage* message, Action* r)
{
	m_steps--;
	if (m_steps < 0) {
		m_status = Status::SUCCESSED;
	}
	else {
		triggerMove();
	}
}

/**
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::Move2Player::init(void* data)
{
	m_status = Status::RUNNING;

	if (m_player == nullptr) {
		m_player = g_gaWorld.getEntity("player");
	}

	m_steps = -1;

	// broadcast the beginning of the move (for animation)
	m_entity->sendMessage(gaMessage::START_MOVE);		// start the entity animation

	locatePlayer();
	m_steps = 30;
	triggerMove();
}

void DarkForces::Behavior::Move2Player::dispatchMessage(gaMessage* message, Action* r)
{
	switch (message->m_action) {
	case gaMessage::Action::MOVE:
		onMove(message, r);
		break;

	case gaMessage::Action::COLLIDE:
		m_entity->sendMessage(gaMessage::Action::END_MOVE);	// stop the entity animation
		return failed(r);
		break;
	}
	BehaviorNode::execute(r);
}