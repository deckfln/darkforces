#include "InfElevatorTranslate.h"

void DarkForces::Component::InfElevatorTranslate::moveTo(float z_lvl)
{
	// move the sector the elevator is based on (for collision detection)
	GameEngine::Transform* tr = m_entity->pTransform();
	tr->m_position = m_entity->position();
	tr->m_position.y = z_lvl / 10.0f;
	m_entity->sendMessage(m_entity->name(), gaMessage::Action::WANT_TO_MOVE, gaMessage::Flag::PUSH_ENTITIES, tr);
}

DarkForces::Component::InfElevatorTranslate::InfElevatorTranslate(dfElevator::Type kind, dfSector* sector):
	InfElevator(kind, sector)
{
}