#include "InfElevatorDoor.h"
#include "InfElevatorDoor.h

void DarkForces::Component::InfElevatorDoor::moveTo(float ambient)
{
	// move the sector the elevator is based on (for collision detection)
	m_transforms.m_position = position();
	m_transforms.m_position.y = z_lvl / 10.0f;
	sendMessage(this->m_name, gaMessage::Action::WANT_TO_MOVE, gaMessage::Flag::PUSH_ENTITIES, &m_transforms);
}

DarkForces::Component::InfElevatorDoor::InfElevatorDoor(dfSector* sector):
	InfElevator(sector)
{
	m_type = dfElevator::Type::DOOR;
}