#include "InfElevatorTranslate.h"

#include "../../dfFileSystem.h"

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

/**
 * build the dfMesh of the elevator
 */
dfMesh* DarkForces::Component::InfElevatorTranslate::buildMesh(void)
{
	// fix the parent sector floor and ceiling based on the elevator
	switch (m_type) {
	case dfElevator::Type::INV:
	case dfElevator::Type::DOOR:
		m_pSector->staticCeilingAltitude(m_zmax);
		break;

	case dfElevator::Type::MOVE_FLOOR:
		m_pSector->staticFloorAltitude(m_zmin);
		break;

	case dfElevator::Type::BASIC:
		m_pSector->staticCeilingAltitude(m_zmax);
		break;

	case dfElevator::Type::MOVE_CEILING:
		m_pSector->staticCeilingAltitude(m_zmax);
		m_pSector->ceiling(m_pSector->referenceFloor());
		break;
	}

	return DarkForces::Component::InfElevator::buildMesh();
}

/**
 * deal with messages specifically for translation elevators
 */
void DarkForces::Component::InfElevatorTranslate::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DF_MSG_EVENT:
		// m_action => event from DF
		uint32_t event = message->m_value;

		if ((m_eventMask & event) != 0) {
			// trigger the program if it fits the eventMask
			m_entity->sendInternalMessage(DF_MESSAGE_TRIGGER);
		}
		break;
	}

	DarkForces::Component::InfElevator::dispatchMessage(message);
}
