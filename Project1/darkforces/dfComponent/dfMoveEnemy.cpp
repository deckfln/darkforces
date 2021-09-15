#include "dfMoveEnemy.h"

DarkForces::Component::MoveEnemy::MoveEnemy():
	GameEngine::Component::BehaviorTree(&m_satnav)
{
	m_satnav.speed(1.0f);
}
