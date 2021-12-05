#include "InfElevatorRotate.h"

#include "../../dfMesh.h"
#include "../../../config.h"

void DarkForces::Component::InfElevatorRotate::moveTo(float z_lvl)
{
	GameEngine::Transform* tr = m_entity->pTransform();
	tr->m_position = glm::vec3(0, glm::radians(z_lvl), 0);
	m_entity->sendInternalMessage(gaMessage::ROTATE, gaMessage::Flag::ROTATE_VEC3, tr);

	// change the sound 'opacity' of the elevator (door) base don openess
	dfSector* sector = dynamic_cast<dfSector*>(m_entity);
	float openess = 0;
	float a;
	float p;

	switch (m_type) {
	case DarkForces::Component::InfElevator::Type::MORPH_SPIN1:
	case DarkForces::Component::InfElevator::Type::MORPH_SPIN2:
		a = m_zmax - m_zmin;
		p = z_lvl - m_zmin;
		break;

	default:
		a = p = 1.0f;
		__debugbreak();
	}

	if (p > 0) {
		openess = p / a;
	}
	m_entity->sendMessage(gaMessage::Action::VOLUME_TRANSPARENCY, sector->soundVolume(), openess);
}

DarkForces::Component::InfElevatorRotate::InfElevatorRotate(DarkForces::Component::InfElevator::Type kind, dfSector* sector, bool smart):
	InfElevator(kind, sector, smart)
{
}

/**
 * relocate the dfMesh of the elevator inv
 */
void DarkForces::Component::InfElevatorRotate::relocateMesh(dfMesh* mesh)
{
	// move the vertices around the center (in level space)
	m_center.z = m_pSector->referenceFloor();
	mesh->moveVertices(m_center);
}

/**
 * build the dfMesh of the elevator
 */
dfMesh* DarkForces::Component::InfElevatorRotate::buildMesh(void)
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
 * deal with messages specifically for translation elevators
 */
void DarkForces::Component::InfElevatorRotate::dispatchMessage(gaMessage* message)
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