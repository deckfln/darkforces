#include "InfElevatorLight.h"

#include "../../../gaEngine/World.h"
#include "../../dfSector.h"
#include "../../../config.h"

/**
 * set the ambient color of the sector
 */
void DarkForces::Component::InfElevatorLight::moveTo(float ambient)
{
	if (m_pSector && m_animate) {
		m_pSector->changeAmbient(ambient);
	}
}

DarkForces::Component::InfElevatorLight::InfElevatorLight(dfSector* sector) :
	DarkForces::Component::InfElevator(DarkForces::Component::InfElevator::Type::CHANGE_LIGHT, sector)
{
	gaComponent::m_type = DF_COMPONENT_INF_ELEVATOR_LIGHT;

}

/**
 * let an entity deal with a situation
 */
void DarkForces::Component::InfElevatorLight::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::HIDE:
		m_animate = false;
		break;
	case gaMessage::Action::UNHIDE:
		m_animate = true;

		// change_light are auto start
		g_gaWorld.sendMessageDelayed(m_entity->name(), m_entity->name(), gaMessage::TIMER, 0, nullptr);
		break;
	}
}
