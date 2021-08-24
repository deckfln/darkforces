#include "InfElevatorDoor.h"

#include "../../../dfMesh.h"

DarkForces::Component::InfElevatorDoor::InfElevatorDoor(dfSector* sector):
	InfElevatorTranslate(DarkForces::Component::InfElevator::Type::DOOR, sector, true)
{
}

/**
 * build the dfMesh of the elevator
 */
dfMesh* DarkForces::Component::InfElevatorDoor::buildMesh(void)
{
	prepareMesh();

	meshData(
		0,
		m_zmax - m_zmin,
		DFWALL_TEXTURE_TOP,
		true,
		dfWallFlag::ALL);

	m_pSector->setAABBtop(m_zmax);

	m_pSector->staticCeilingAltitude(m_zmax);
	return DarkForces::Component::InfElevatorTranslate::buildMesh();
}

/**
 * relocate the dfMesh of the elevator inv
 */
void DarkForces::Component::InfElevatorDoor::relocateMesh(dfMesh *mesh)
{
	// for these elevator, Z is defined by the elevator, so center on XY (in level space)
	mesh->centerOnGeometryXZ(m_center);
}