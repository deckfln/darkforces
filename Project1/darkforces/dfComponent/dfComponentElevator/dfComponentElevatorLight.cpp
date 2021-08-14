#include "dfComponentElevatorLight.h"

#include "../../dfElevator.h"
#include "../../../gaEngine/World.h"
#include "../../dfSector.h"

/**
 * set the ambient color of the sector
 */
void DarkForces::Component::ElevatorLight::moveTo(float ambient)
{
	if (m_pSector && m_pSector->visible()) {
		m_pSector->changeAmbient(ambient);
	}
}

DarkForces::Component::ElevatorLight::ElevatorLight(dfSector* sector) :
	DarkForces::Component::Elevator(sector)
{
	m_type = dfElevator::Type::CHANGE_LIGHT;

	// change_light are auto start
	g_gaWorld.sendMessageDelayed(m_entity->name(), m_entity->name(), gaMessage::TIMER, 0, nullptr);
}