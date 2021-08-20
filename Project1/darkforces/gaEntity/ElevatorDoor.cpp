#include "ElevatorDoor.h"

#include "../../gaEngine/gaMessage.h"
#include "../../gaEngine/gaComponent/gaComponentMesh.h"

#include "../dfLogicStop.h"
#include "../dfLogicTrigger.h"
#include "../dfMesh.h"
#include "../dfComponent/InfElevator/InfElevatorTranslate.h"

static const std::string hold = "hold";
static const std::string door = "door";
static const std::string switch1 = "switch1";

DarkForces::Entity::ElevatorDoor::ElevatorDoor(dfSector* sector):
	gaEntity(DF_ENTITY_ELEVATOR, sector->name())
{
	m_physical = true;
	m_gravity = false;
	m_collideSectors = false;
	m_hasCollider = true;
	m_defaultCollision = gaMessage::Flag::PUSH_ENTITIES;

	DarkForces::Component::InfElevatorTranslate* elevator = new DarkForces::Component::InfElevatorTranslate(dfElevator::Type::DOOR, sector);
	dfLogicStop* closed = new dfLogicStop(sector, sector->referenceFloor(), hold);
	dfLogicStop* opened = new dfLogicStop(sector, sector->referenceCeiling(), 5000);

	elevator->addStop(closed);
	elevator->addStop(opened);

	dfMesh* mesh = elevator->buildMesh();
	GameEngine::ComponentMesh* m_component = new GameEngine::ComponentMesh(mesh);

	addComponent(m_component, gaEntity::Flag::DELETE_AT_EXIT);
	addComponent(elevator, gaEntity::Flag::DELETE_AT_EXIT);

	//only init the elevator at the end, AFTER the entity position is forced by the Mesh 
	elevator->gotoStop(0);

	// create a trigger based on the full sector (already registered in the elevator)
	m_trigger = new dfLogicTrigger(switch1, sector->name(), 0);

	// once the elevator closes, send a DONE message to the trigger
	closed->message(new gaMessage(DF_MESSAGE_DONE, 0, m_trigger->name()));

	m_trigger->config();
}

DarkForces::Entity::ElevatorDoor::~ElevatorDoor()
{
	delete m_trigger;
}
