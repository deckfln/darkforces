#include "dfMoveEnemyTo.h"

#include <tinyxml2.h>

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/Physics.h"

#include "../dfComponent.h"
#include "../dfLogicTrigger.h"
#include "../dfComponent/InfElevator.h"

/**
 * run the node logic
 */
enum Child {
	init = -1,
	move_to,
	wait_door,
	open_door
};

DarkForces::Behavior::MoveEnemyTo::MoveEnemyTo(const char* name):
	GameEngine::BehaviorNode(name)
{
}

GameEngine::BehaviorNode* DarkForces::Behavior::MoveEnemyTo::clone(GameEngine::BehaviorNode* p)
{
	DarkForces::Behavior::MoveEnemyTo* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::MoveEnemyTo*>(p);
	}
	else {
		cl = new DarkForces::Behavior::MoveEnemyTo(m_name);
	}
	return cl;
}

BehaviorNode* DarkForces::Behavior::MoveEnemyTo::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	return new DarkForces::Behavior::MoveEnemyTo(name);
}


void DarkForces::Behavior::MoveEnemyTo::init(void* data)
{
	m_destination = m_tree->blackboard<glm::vec3>("static_position");
	BehaviorNode::init(&m_destination);
}


void DarkForces::Behavior::MoveEnemyTo::execute(Action* r)
{
	BehaviorNode::Status childStatus;
	
	r->action = Status::RUNNING;

	if (m_runningChild >= 0) {
		childStatus = m_children[m_runningChild]->status();
	}

	switch (m_runningChild) {
	case Child::init:
		return startChild(r, Child::move_to, &m_destination);

	case Child::move_to: {
		if (childStatus == GameEngine::BehaviorNode::Status::SUCCESSED) {
			// we reached the destination
			return succeeded(r);
		}

		// the move_to node failed, it probably collided with something
		struct GameEngine::Physics::CollisionList& collidedList = m_tree->blackboard<struct GameEngine::Physics::CollisionList>("lastCollision");
		if (collidedList.size == 0) {
			return failed(r);
		}

		DarkForces::Component::InfElevator* elevator = nullptr;
		gaEntity* collided;
		for (auto i = 0; i < collidedList.size; i++) {
			collided = collidedList.entities[i];
			elevator = static_cast<DarkForces::Component::InfElevator*>(collided->findComponent(DF_COMPONENT_INF_ELEVATOR));

			if (elevator != nullptr) {
				break;
			}
		}

		if (elevator) {
			m_tree->blackboard("wait_elevator", elevator);

			// so we are colliding with an elevator, check the status of the elevator
			switch (elevator->status()) {
			case Component::InfElevator::Status::MOVE:
			case Component::InfElevator::Status::WAIT:
				// wait for the elevator to finish its move, maybe it is opening
				return startChild(r, Child::wait_door, elevator);

			case Component::InfElevator::Status::HOLD:
				return startChild(r, Child::open_door, elevator);

			case Component::InfElevator::Status::TERMINATED:
				// the elevator will not move any more, so we have to cancel the move
				// whoever asked me to move will notice this is the end
				m_status = Status::FAILED;
			}
		}
		else {
			// we collided with a wall 
			m_status = Status::FAILED;
		}
		break; }

	case wait_door:
		if (childStatus == Status::SUCCESSED) {
			// now that the door finished moving, restart the move
			return startChild(r, Child::move_to, &m_destination);
		}
		m_status = Status::FAILED;
		break;

	case open_door:
		if (childStatus == Status::SUCCESSED) {
			// now that the door is open, restart the move
			return startChild(r, Child::move_to, &m_destination);
		}
		m_status = Status::FAILED;
		break;
	}

	BehaviorNode::execute(r);
}

/**
 * flight recorder status
 */
uint32_t DarkForces::Behavior::MoveEnemyTo::recordState(void* record)
{
	FlightRecorder::MoveEnemyTo* r = static_cast<FlightRecorder::MoveEnemyTo*>(record);

	BehaviorNode::recordState(record);

	uint32_t len = sizeof(FlightRecorder::MoveEnemyTo);
	r->destination = m_destination;

	r->node.size = len;
	return r->node.size;
}

uint32_t DarkForces::Behavior::MoveEnemyTo::loadState(void* record)
{
	BehaviorNode::loadState(record);

	FlightRecorder::MoveEnemyTo* r = static_cast<FlightRecorder::MoveEnemyTo*>(record);

	m_destination = r->destination;

	return r->node.size;
}
