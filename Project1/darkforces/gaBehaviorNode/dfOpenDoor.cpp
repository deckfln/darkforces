#include "dfOpenDoor.h"

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/World.h"
#include "../../gaEngine/gaNavMesh.h"

#include "../dfSector.h"
#include "../dfLogicTrigger.h"

DarkForces::Behavior::OpenDoor::OpenDoor(const char* name):
	GameEngine::BehaviorNode(name)
{
}

enum Child {
	init = -1,
	goto_trigger,
	wait_door
};

void DarkForces::Behavior::OpenDoor::init(void *)
{
	m_runningChild = Child::init;
	m_status = Status::RUNNING;
}

/**
 * move to the next trigger
 */
GameEngine::BehaviorNode* DarkForces::Behavior::OpenDoor::nextNode(void)
{
	if (m_status != Status::RUNNING) {
		return this;
	}

	switch (m_runningChild) {
	case Child::init:
		return startChild(Child::goto_trigger, nullptr);

	case Child::goto_trigger:
		if (m_children[m_runningChild]->status() == Status::SUCCESSED) {
			return startChild(Child::wait_door, nullptr);
		}

		return exitChild(Status::FAILED);
		break;

	case Child::wait_door:
		if (m_children[m_runningChild]->status() == Status::SUCCESSED) {
			return exitChild(Status::SUCCESSED);
		}
	}

	return this;
}