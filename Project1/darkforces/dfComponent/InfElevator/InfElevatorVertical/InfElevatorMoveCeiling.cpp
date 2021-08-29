#include "InfElevatorMoveCeiling.h"

#include "../../../dfMesh.h"

DarkForces::Component::InfElevatorMoveCeiling::InfElevatorMoveCeiling(dfSector* sector):
	InfElevatorTranslate(DarkForces::Component::InfElevator::Type::MOVE_CEILING, sector, true)
{
	// change the status of the entity sector to make it a full interactive entity
	sector->physical(true);
	sector->gravity(false);
	sector->collideSectors(false);
	sector->hasCollider(true);
	sector->defaultCollision(gaMessage::Flag::PUSH_ENTITIES);
	sector->displayAABBox();
}

/**
 * build the dfMesh of the elevator
 */
dfMesh* DarkForces::Component::InfElevatorMoveCeiling::buildMesh(void)
{
	prepareMesh();

	meshData(0,
		(m_zmax - m_zmin),
		DFWALL_TEXTURE_TOP,
		false,
		dfWallFlag::ALL);

	m_pSector->setAABBbottom(m_zmax);

	m_pSector->staticCeilingAltitude(m_zmax);
	m_pSector->ceiling(m_pSector->referenceFloor());
	return DarkForces::Component::InfElevatorTranslate::buildMesh();
}

/**
 * relocate the dfMesh of the elevator inv
 */
void DarkForces::Component::InfElevatorMoveCeiling::relocateMesh(dfMesh *mesh)
{
	// for these elevator, Z is defined by the elevator, so center on XY (in level space)
	mesh->centerOnGeometryXZ(m_center);
}