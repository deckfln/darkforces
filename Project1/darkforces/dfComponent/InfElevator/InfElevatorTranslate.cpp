#include "InfElevatorTranslate.h"

#include "../../dfFileSystem.h"
#include "../../../config.h"

void DarkForces::Component::InfElevatorTranslate::moveTo(float z_lvl)
{
	// move the sector the elevator is based on (for collision detection)
	GameEngine::Transform* tr = m_entity->pTransform();
	tr->m_position = m_entity->position();
	tr->m_position.y = z_lvl / 10.0f;
	m_entity->sendMessage(m_entity->name(), gaMessage::Action::WANT_TO_MOVE, 0, tr);
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
