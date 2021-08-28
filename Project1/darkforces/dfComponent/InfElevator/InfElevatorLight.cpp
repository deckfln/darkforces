#include "InfElevatorLight.h"

#include "../../../gaEngine/World.h"
#include "../../dfSector.h"
#include "../../../config.h"

/**
 * set the ambient color of the sector
 */
void DarkForces::Component::InfElevatorLight::moveTo(float ambient)
{
	if (m_pSector && m_pSector->visible()) {
		m_pSector->changeAmbient(ambient);
	}
}

DarkForces::Component::InfElevatorLight::InfElevatorLight(dfSector* sector) :
	DarkForces::Component::InfElevator(DarkForces::Component::InfElevator::Type::CHANGE_LIGHT, sector)
{
	gaComponent::m_type = DF_COMPONENT_INF_ELEVATOR_LIGHT;

	// change_light are auto start
	g_gaWorld.sendMessageDelayed(m_entity->name(), m_entity->name(), gaMessage::TIMER, 0, nullptr);
}