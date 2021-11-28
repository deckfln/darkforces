#include "dfOpenDoor.h"

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/World.h"
#include "../../gaEngine/gaNavMesh.h"

#include "../dfSector.h"
#include "../dfLogicTrigger.h"

DarkForces::Behavior::OpenDoor::OpenDoor(const char* name):
	GameEngine::Behavior::Sequence(name)
{
}

enum Child {
	init = -1,
	goto_trigger,
	wait_door
};

void DarkForces::Behavior::OpenDoor::init(void *data)
{
	m_collision = data;
	m_tree->blackboard("lastCollision", nullptr);
	GameEngine::Behavior::Sequence::init(data);
}

BehaviorNode* DarkForces::Behavior::OpenDoor::create(const char* name)
{
	return new DarkForces::Behavior::OpenDoor(name);
}
