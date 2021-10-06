#include "dfEnemyAttack.h"

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../config.h"

DarkForces::Behavior::EnemyAttack::EnemyAttack(const char* name):
	BehaviorNode(name)
{
}

/**
 * locate the player
 */
void DarkForces::Behavior::EnemyAttack::locatePlayer(void)
{
	// get the player position
	m_direction = glm::normalize(m_player->position() - m_entity->position());

	// turn toward the player
	m_entity->sendMessage(gaMessage::LOOK_AT, -m_direction);

	// and fire
	m_entity->sendMessage(DarkForces::Message::FIRE, 0, (void*)&m_direction);
}

/**
 *
 */
void DarkForces::Behavior::EnemyAttack::triggerMove(void)
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
void DarkForces::Behavior::EnemyAttack::onMove(gaMessage* message, Action* r)
{
	m_steps--;
	if (m_steps < 0) {
		locatePlayer();
		triggerMove();
		m_steps=30;
	}
	else {
		triggerMove();
	}
}

/**
 * on init, look for the player and fire on him
 */
void DarkForces::Behavior::EnemyAttack::init(void* data)
{
	m_status = Status::RUNNING;

	if (m_player == nullptr) {
		m_player = g_gaWorld.getEntity("player");
	}

	m_steps = -1;

	// broadcast the beginning of the move (for animation)
	m_entity->sendDelayedMessage(gaMessage::START_MOVE);

	onMove(nullptr, nullptr);
}

void DarkForces::Behavior::EnemyAttack::dispatchMessage(gaMessage* message, Action* r)
{
	switch (message->m_action) {
	case gaMessage::Action::MOVE:
		onMove(message, r);
		break;

	case gaMessage::Action::COLLIDE:
		return failed(r);
		break;
	}
	BehaviorNode::dispatchMessage(message, r);
}

