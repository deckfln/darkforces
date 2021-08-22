#include "InfElevatorMorphSpin1.h"

#include "../../../dfMesh.h"

DarkForces::Component::InfElevatorMorphSpin1::InfElevatorMorphSpin1(dfSector* sector):
	InfElevatorRotate(dfElevator::Type::MORPH_SPIN1, sector, true)
{
	// change the status of the entity sector to make it a full interactive entity
	sector->physical(true);
	sector->gravity(false);
	sector->collideSectors(false);
	sector->hasCollider(true);
	sector->defaultCollision(gaMessage::Flag::PUSH_ENTITIES);
	sector->displayAABBox();

	meshData(
		sector->staticFloorAltitude(),
		sector->staticCeilingAltitude(),
		DFWALL_TEXTURE_MID,
		false,
		dfWallFlag::MORPHS_WITH_ELEV);
}