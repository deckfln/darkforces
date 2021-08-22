#include "InfElevatorMorphMove1.h"

#include "../../../dfMesh.h"

DarkForces::Component::InfElevatorMorphMove1::InfElevatorMorphMove1(dfSector* sector):
	InfElevatorHorizontal(dfElevator::Type::MORPH_MOVE1, sector, true)
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