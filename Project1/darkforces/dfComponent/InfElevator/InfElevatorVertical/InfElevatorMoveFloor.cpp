#include "InfElevatorMoveFloor.h"

#include "../../../dfMesh.h"

DarkForces::Component::InfElevatorMoveFloor::InfElevatorMoveFloor(dfSector* sector):
	InfElevatorTranslate(DarkForces::Component::InfElevator::Type::MOVE_FLOOR, sector, false)
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
dfMesh* DarkForces::Component::InfElevatorMoveFloor::buildMesh(void)
{
	prepareMesh();

	meshData(
		-(m_zmax - m_zmin),
		0,
		DFWALL_TEXTURE_BOTTOM,
		false,
		dfWallFlag::ALL);

	m_pSector->setAABBbottom(m_zmin);

	m_pSector->staticFloorAltitude(m_zmin);
	return DarkForces::Component::InfElevatorTranslate::buildMesh();
}

/**
 * relocate the dfMesh of the elevator inv
 */
void DarkForces::Component::InfElevatorMoveFloor::relocateMesh(dfMesh *mesh)
{
	// for these elevator, Z is defined by the elevator, so center on XY (in level space)
	mesh->centerOnGeometryXZ(m_center);
}