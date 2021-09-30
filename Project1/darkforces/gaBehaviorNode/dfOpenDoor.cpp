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
void DarkForces::Behavior::OpenDoor::nextNode(Action *r)
{
	if (m_status != Status::RUNNING) {
		r->action = sAction::RUNNING;
	}

	switch (m_runningChild) {
	case Child::init:
		r->action = sAction::START_CHILD;
		r->child = Child::goto_trigger;
		r->data = m_collision;
		return;

	case Child::goto_trigger:
		if (m_children[m_runningChild]->status() == Status::SUCCESSED) {
			r->action = sAction::START_CHILD;
			r->child = Child::wait_door;
			r->data = m_collision;
			return;
		}
		r->action = sAction::EXIT;
		r->status = Status::FAILED;
		return;
		break;

	case Child::wait_door:
		if (m_children[m_runningChild]->status() == Status::SUCCESSED) {
			r->action = sAction::EXIT;
			r->status = Status::SUCCESSED;
			return;
		}
	}

	r->action = sAction::RUNNING;
}