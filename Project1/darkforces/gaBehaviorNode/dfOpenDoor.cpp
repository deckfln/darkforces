#include "dfOpenDoor.h"

#include <tinyxml2.h>

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

GameEngine::BehaviorNode* DarkForces::Behavior::OpenDoor::clone(GameEngine::BehaviorNode* p)
{
	DarkForces::Behavior::OpenDoor* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::OpenDoor*>(p);
	}
	else {
		cl = new DarkForces::Behavior::OpenDoor(m_name);
	}
	GameEngine::Behavior::Sequence::clone(cl);
	return cl;
}

BehaviorNode* DarkForces::Behavior::OpenDoor::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	return new DarkForces::Behavior::OpenDoor(name);
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

