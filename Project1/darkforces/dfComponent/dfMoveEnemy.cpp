#include "dfMoveEnemy.h"

DarkForces::Component::MoveEnemy::MoveEnemy():
	GameEngine::Component::BehaviorTree(&m_move_to)
{
	m_satnav.speed(1.0f);

	m_move_to.tree(this);
	m_move_to.addNode(&m_satnav);
	m_move_to.addNode(&m_wait_door);
	m_move_to.addNode(&m_open_door);
		m_open_door.addNode(&m_goto_trigger);
			m_goto_trigger.addNode(&m_satnav_door);
		m_open_door.addNode(&m_wait_door_2);
}
