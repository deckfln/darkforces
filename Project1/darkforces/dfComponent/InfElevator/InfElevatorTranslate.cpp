#include "InfElevatorTranslate.h"

#include "../../dfFileSystem.h"
#include "../../dfSector.h"
#include "../../dfLogicStop.h"

#include "../../../config.h"

void DarkForces::Component::InfElevatorTranslate::moveTo(float z_lvl)
{
	// move the sector the elevator is based on (for collision detection)
	GameEngine::Transform* tr = m_entity->pTransform();
	tr->m_position = m_entity->position();
	tr->m_position.y = z_lvl / 10.0f;
	m_entity->sendMessage(m_entity->name(), gaMessage::Action::MOVE, 0, &tr->m_position);

	// change the sound 'opacity' of the elevator (door) base don openess
	dfSector* sector = dynamic_cast<dfSector*>(m_entity);
	float openess = 0;
	float h;
	float p;

	switch (m_type) {
	case DarkForces::Component::InfElevator::Type::DOOR:
		h = m_stops[1]->z_position(m_type) - m_stops[0]->z_position(m_type);
		p = z_lvl - m_stops[0]->z_position(m_type);
		break;

	case DarkForces::Component::InfElevator::Type::MOVE_FLOOR:
		h = sector->staticCeilingAltitude() - sector->staticFloorAltitude();
		p = sector->staticCeilingAltitude() - z_lvl;
		break;

	case DarkForces::Component::InfElevator::Type::MOVE_CEILING:
		h = sector->staticCeilingAltitude() - sector->staticFloorAltitude();
		p = sector->staticFloorAltitude() - z_lvl;
		break;

	case DarkForces::Component::InfElevator::Type::INV:
	case DarkForces::Component::InfElevator::Type::BASIC:
		h = sector->staticCeilingAltitude() - sector->staticFloorAltitude();
		p = z_lvl - sector->staticFloorAltitude();
		break;

	default:
		p = h = 1.0f;
		__debugbreak();
	}

	if (p > 0) {
		openess = p / h;
	}
	m_entity->sendMessage(gaMessage::Action::VOLUME_TRANSPARENCY, sector->soundVolume(), openess);
}

DarkForces::Component::InfElevatorTranslate::InfElevatorTranslate(DarkForces::Component::InfElevator::Type kind, dfSector* sector, bool smart):
	InfElevator(kind, sector, smart)
{
}

/**
 * deal with messages specifically for translation elevators
 */
void DarkForces::Component::InfElevatorTranslate::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::EVENT:
		// m_action => event from DF
		uint32_t event = message->m_value;

		if ((m_eventMask & event) != 0) {
			// trigger the program if it fits the eventMask
			m_entity->sendInternalMessage(DarkForces::Message::TRIGGER);
		}
		break;
	}

	DarkForces::Component::InfElevator::dispatchMessage(message);
}
