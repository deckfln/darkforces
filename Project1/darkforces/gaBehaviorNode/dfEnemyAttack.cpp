#include "dfEnemyAttack.h"

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../config.h"

DarkForces::Behavior::EnemyAttack::EnemyAttack(const char* name):
	BehaviorNode(name)
{
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

	// get the player position
	m_position = glm::normalize(m_player->position() - m_entity->position());

	m_entity->sendMessage(DarkForces::Message::FIRE, 0, (void*)&m_position);
}

/**
 *
 */
void DarkForces::Behavior::EnemyAttack::execute(Action* r)
{
	return succeeded(r); // exit immediately
}

void DarkForces::Behavior::EnemyAttack::dispatchMessage(gaMessage* message, Action* r)
{
	switch (message->m_action) {
	case DarkForces::Message::HIT_BULLET:
		break;
	}
	BehaviorNode::dispatchMessage(message, r);
}

