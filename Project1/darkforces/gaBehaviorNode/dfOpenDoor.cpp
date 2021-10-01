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

void DarkForces::Behavior::OpenDoor::init(void *data)
{
	m_runningChild = Child::init;
	m_status = Status::RUNNING;

	m_collision = data;
	m_tree->blackboard("lastCollision", nullptr);
}

/**
 * move to the next trigger
 */
void DarkForces::Behavior::OpenDoor::execute(Action *r)
{
	r->action = Status::RUNNING;

	switch (m_runningChild) {
	case Child::init:
		return startChild(r, Child::goto_trigger, m_collision);

	case Child::goto_trigger:
		if (m_children[m_runningChild]->status() == Status::SUCCESSED) {
			return startChild(r, Child::wait_door, m_collision);
		}
		m_status = Status::FAILED;
		break;

	case Child::wait_door:
		if (m_children[m_runningChild]->status() == Status::SUCCESSED) {
			m_status = Status::SUCCESSED;
		}
		break;
	}

	BehaviorNode::execute(r);
}