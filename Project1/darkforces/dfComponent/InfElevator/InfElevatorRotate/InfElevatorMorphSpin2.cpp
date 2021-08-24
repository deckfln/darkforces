#include "InfElevatorMorphSpin2.h"

#include "../../../dfMesh.h"

DarkForces::Component::InfElevatorMorphSpin2::InfElevatorMorphSpin2(dfSector* sector):
	InfElevatorRotate(DarkForces::Component::InfElevator::Type::MORPH_SPIN2, sector, true)
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