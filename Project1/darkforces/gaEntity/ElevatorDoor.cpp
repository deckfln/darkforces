#include "ElevatorDoor.h"

#include "../../gaEngine/gaMessage.h"
#include "../../gaEngine/gaComponent/gaComponentMesh.h"
#include "../../gaEngine/gaComponent/gaSound.h"

#include "../dfConfig.h"
#include "../dfFileSystem.h"
#include "../dfLogicStop.h"
#include "../dfLogicTrigger.h"
#include "../dfMesh.h"
#include "../dfVOC.h"
#include "../dfComponent/Trigger.h"
#include "../dfComponent/InfElevator/InfElevatorVertical/InfElevatorDoor.h"

static const std::string hold = "hold";
static const std::string door = "door";
static const std::string switch1 = "switch1";

// predefined sounds for the doors
static const char* sounds[] = {
	"door.voc",
	"",
	""
};

static dfVOC* cache[3] = { nullptr, nullptr, nullptr };

/**
 *
 */
DarkForces::Entity::ElevatorDoor::ElevatorDoor(dfSector* sector):
	gaEntity(DarkForces::ClassID::Elevator, sector->name())
{
	m_physical = true;
	m_gravity = false;
	m_collideSectors = false;
	m_hasCollider = true;
	m_movable = false;

	// prepare the elevator component
	DarkForces::Component::InfElevatorDoor* elevator = new DarkForces::Component::InfElevatorDoor(sector);
	dfLogicStop* closed = new dfLogicStop(sector, sector->referenceFloor(), hold);
	dfLogicStop* opened = new dfLogicStop(sector, sector->referenceCeiling(), 5000);

	elevator->addStop(closed);
	elevator->addStop(opened);

	// prepare the mesh component
	dfMesh* mesh = elevator->buildMesh();
	GameEngine::ComponentMesh* cmesh = new GameEngine::ComponentMesh(mesh);

	// prepare the sound component
	GameEngine::Component::Sound* sound = new GameEngine::Component::Sound();
	sound->addSound(DarkForces::Component::InfElevator::Sound::START, loadVOC("door.voc")->sound());

	// prepare the Trigger component (to handle key press)
	DarkForces::Component::Trigger* trigger = new DarkForces::Component::Trigger();
	elevator->addTrigger(trigger);

	addComponent(cmesh, gaEntity::Flag::DELETE_AT_EXIT);
	addComponent(elevator, gaEntity::Flag::DELETE_AT_EXIT);
	addComponent(sound, gaEntity::Flag::DELETE_AT_EXIT);
	addComponent(trigger, gaEntity::Flag::DELETE_AT_EXIT);

	//only init the elevator at the end, AFTER the entity position is forced by the Mesh 
	elevator->gotoStop(0);
}

DarkForces::Entity::ElevatorDoor::~ElevatorDoor()
{
}
