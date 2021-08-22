#include "InfElevatorBasic.h"

#include "../../../dfMesh.h"

DarkForces::Component::InfElevatorBasic::InfElevatorBasic(dfSector* sector):
	InfElevatorTranslate(dfElevator::Type::BASIC, sector, true)
{
	// change the status of the entity sector to make it a full interactive entity
	sector->physical(true);
	sector->gravity(false);
	sector->collideSectors(false);
	sector->hasCollider(true);
	sector->defaultCollision(gaMessage::Flag::PUSH_ENTITIES);
	sector->displayAABBox();

	if (m_zmin == INFINITY) {
		m_zmin = sector->staticFloorAltitude();
		m_zmax = sector->staticCeilingAltitude();
	}

	meshData(
		0,
		-(m_zmax - m_zmin),
		DFWALL_TEXTURE_TOP,
		true,
		dfWallFlag::ALL);

	sector->setAABBtop(m_zmax);
}

/**
 * build the dfMesh of the elevator
 */
dfMesh* DarkForces::Component::InfElevatorBasic::buildMesh(void)
{
	m_pSector->staticCeilingAltitude(m_zmax);
	return DarkForces::Component::InfElevatorTranslate::buildMesh();
}

/**
 * relocate the dfMesh of the elevator inv
 */
void DarkForces::Component::InfElevatorBasic::relocateMesh(dfMesh *mesh)
{
	// for these elevator, Z is defined by the elevator, so center on XY (in level space)
	mesh->centerOnGeometryXZ(m_center);
}