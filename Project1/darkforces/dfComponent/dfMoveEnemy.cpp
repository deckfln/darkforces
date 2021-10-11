#include "dfMoveEnemy.h"

DarkForces::Component::MoveEnemy::MoveEnemy():
	GameEngine::Component::BehaviorTree(&m_waitIdle)
{
	m_satnav.speed(1.0f);

	m_waitIdle.tree(this);
	m_waitIdle.init(nullptr);

	m_waitIdle.addNode(&m_attack);
		m_attack.addNode(&m_move2player);
			m_move2player.addNode(&m_move2);
		m_attack.addNode(&m_shootPlayer);

	m_waitIdle.addNode(&m_move_to);
		m_move_to.addNode(&m_satnav);
		m_move_to.addNode(&m_wait_door);
		m_move_to.addNode(&m_open_door);
			m_open_door.addNode(&m_goto_trigger);
				m_goto_trigger.addNode(&m_satnav_door);
			m_open_door.addNode(&m_wait_door_2);
}
