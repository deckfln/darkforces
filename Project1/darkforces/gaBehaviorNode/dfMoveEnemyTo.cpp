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

BehaviorNode* DarkForces::Behavior::MoveEnemyTo::create(const char* name, tinyxml2::XMLElement* element)
{
	return new DarkForces::Behavior::MoveEnemyTo(name);
}


void DarkForces::Behavior::MoveEnemyTo::init(void* data)
{
	m_destination = *(static_cast<glm::vec3*>(data));
	BehaviorNode::init(data);
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
		struct GameEngine::Physics::CollisionList* collidedList = m_tree->blackboard<struct GameEngine::Physics::CollisionList>("lastCollision");
		if (collidedList != nullptr && collidedList->size == 0) {
			return failed(r);
		}

		DarkForces::Component::InfElevator* elevator = nullptr;
		if (collidedList != nullptr) {
			gaEntity* collided;
			for (auto i = 0; i < collidedList->size; i++) {
				collided = collidedList->entities[i];
				elevator = static_cast<DarkForces::Component::InfElevator*>(collided->findComponent(DF_COMPONENT_INF_ELEVATOR));

				if (elevator != nullptr) {
					break;
				}
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

	DarkForces::Component::InfElevator* elevator = m_tree->blackboard<DarkForces::Component::InfElevator>("wait_elevator");
	if (elevator) {
		gaEntity* entity = elevator->entity();
		strcpy_s(&r->wait_elevator[0], 1024, entity->name().c_str());
		len += entity->name().size() + 1;
	}
	else {
		r->wait_elevator[0] = 0;
	}

	r->node.size = len;
	return r->node.size;
}

uint32_t DarkForces::Behavior::MoveEnemyTo::loadState(void* record)
{
	BehaviorNode::loadState(record);

	FlightRecorder::MoveEnemyTo* r = static_cast<FlightRecorder::MoveEnemyTo*>(record);

	m_destination = r->destination;

	if (r->wait_elevator[0] != 0) {
		gaEntity* entity = g_gaWorld.getEntity(r->wait_elevator);
		DarkForces::Component::InfElevator* elevator = dynamic_cast<DarkForces::Component::InfElevator*>(entity->findComponent(DF_COMPONENT_INF_ELEVATOR));
		m_tree->blackboard("wait_elevator", elevator);
	}
	return r->node.size;
}
