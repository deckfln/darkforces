#include "InfElevatorHorizontal.h"

#include "../../dfMesh.h"
#include "../../dfLevel.h"
#include "../../../config.h"

void DarkForces::Component::InfElevatorHorizontal::moveTo(float z_lvl)
{
	glm::vec3 p = m_center + m_move * z_lvl;
	dfLevel::level2gl(p);
	m_entity->sendMessage(gaMessage::MOVE, 0, p);

	// change the sound 'opacity' of the elevator (door) base don openess
	dfSector* sector = dynamic_cast<dfSector*>(m_entity);
	float openess = 0;
	float a;
	float p1;

	switch (m_type) {
	case DarkForces::Component::InfElevator::Type::MORPH_MOVE1:
		a = m_zmax - m_zmin;
		p1 = z_lvl - m_zmin;
		break;

	default:
		a = p1 = 1.0f;
		__debugbreak();
	}

	if (p1 > 0) {
		openess = p1 / a;
	}
	m_entity->sendMessage(gaMessage::Action::VOLUME_TRANSPARENCY, sector->soundVolume(), openess);
}

DarkForces::Component::InfElevatorHorizontal::InfElevatorHorizontal(DarkForces::Component::InfElevator::Type kind, dfSector* sector, bool smart):
	InfElevator(kind, sector, smart)
{
}

/**
 * for morph_move1, convert angle translation to a vector translation
 */
void DarkForces::Component::InfElevatorHorizontal::angle(float angle)
{
	angle = glm::radians(angle);	// convert degrees to radians
	m_move = glm::vec3(-sin(angle), cos(angle), 0);
}

/**
 * build the dfMesh of the elevator
 */
dfMesh* DarkForces::Component::InfElevatorHorizontal::buildMesh(void)
{
	// remove all non-portal walls. These walls will be stored on the Elevator mesh
	m_pSector->removeHollowWalls();

	// if the sector is included in another one (eg slider_sw on secbase)
	// remove the hollows from the parent sector
	dfSector* parent = m_pSector->isIncludedIn();
	if (parent != nullptr) {
		parent->removeHollowWalls();
	}

	return DarkForces::Component::InfElevator::buildMesh();
}

/**
 * relocate the dfMesh of the elevator inv
 */
void DarkForces::Component::InfElevatorHorizontal::relocateMesh(dfMesh* mesh)
{
	// for this elevator, move along an axes from a center, so center on XYZ (in level space)
	mesh->centerOnGeometryXYZ(m_center);
}


/**
 * deal with messages specifically for translation elevators
 */
void DarkForces::Component::InfElevatorHorizontal::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case DarkForces::Message::EVENT:
		// m_action => event from DF
		uint32_t event = message->m_value;

		if ((m_eventMask & event) != 0) {
			// trigger the program if it fits the eventMask
			m_entity->sendMessage(DarkForces::Message::TRIGGER);
		}
		break;
	}

	DarkForces::Component::InfElevator::dispatchMessage(message);
}
