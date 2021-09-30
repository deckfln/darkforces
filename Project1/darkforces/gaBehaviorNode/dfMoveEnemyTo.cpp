#include "dfMoveEnemyTo.h"

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaBehaviorTree.h"
#include "../../gaEngine/Physics.h"

#include "../dfLogicTrigger.h"
#include "../dfComponent/InfElevator.h"

DarkForces::Behavior::MoveEnemyTo::MoveEnemyTo(const char* name):
	GameEngine::BehaviorNode(name)
{
}

/**
 * run the node logic
 */
enum Child {
	init = -1,
	move_to,
	wait_door,
	open_door
};

/**
 * run the node actions
 */
void DarkForces::Behavior::MoveEnemyTo::dispatchMessage(gaMessage* message, Action*r)
{
	if (message->m_action == gaMessage::Action::SatNav_GOTO) {
		if (message->m_extra == nullptr) {
			m_destination = message->m_v3value;
		}
		else {
			m_destination = *(static_cast<glm::vec3*>(message->m_extra));
		}

		m_runningChild = Child::init;

		m_status = Status::RUNNING;
	}

	r->action = sAction::NEXT_NODE;
}


void DarkForces::Behavior::MoveEnemyTo::nextNode(Action* r)
{
	GameEngine::BehaviorNode::Status childStatus;
	
	if (m_status != Status::RUNNING) {
		r->action = sAction::RUNNING;
		return;
	}

	if (m_runningChild >= 0) {
		childStatus = m_children[m_runningChild]->status();
	}

	switch (m_runningChild) {
	case Child::init:
		r->action = sAction::START_CHILD;
		r->child = Child::move_to;
		r->data = &m_destination;
		return;

	case Child::move_to: {
		if (childStatus == GameEngine::BehaviorNode::Status::SUCCESSED) {
			// we reached the destination
			r->action = sAction::EXIT;
			r->status = Status::SUCCESSED;
			return;
		}

		// the move_to node failed, it probably collided with something
		struct GameEngine::Physics::CollisionList* collidedList = static_cast<struct GameEngine::Physics::CollisionList*>(m_tree->blackboard("lastCollision"));
		if (collidedList != nullptr && collidedList->size == 0) {
			r->action = sAction::EXIT;
			r->status = Status::FAILED;
			return;
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
				r->action = sAction::START_CHILD;
				r->child = Child::wait_door;
				r->data = elevator;
				return;

			case Component::InfElevator::Status::HOLD:
				r->action = sAction::START_CHILD;
				r->child = Child::open_door;
				r->data = elevator;
				return;

			case Component::InfElevator::Status::TERMINATED:
				// the elevator will not move any more, so we have to cancel the move
				// whoever asked me to move will notice this is the end
				r->action = sAction::EXIT;
				r->status = Status::FAILED;
				return;
			}
		}
		else {
			// we collided with a wall 
			r->action = sAction::EXIT;
			r->status = Status::FAILED;
			return;
		}
		break; }

	case wait_door:
		if (childStatus == Status::SUCCESSED) {
			// now that the door finished moving, restart the move
			r->action = sAction::START_CHILD;
			r->child = Child::move_to;
			r->data = &m_destination;
			return;
		}

		r->action = sAction::EXIT;
		r->status = Status::FAILED;
		return;

	case open_door:
		if (childStatus == Status::SUCCESSED) {
			// now that the door is open, restart the move
			r->action = sAction::START_CHILD;
			r->child = Child::move_to;
			r->data = &m_destination;
			return;
		}
		r->action = sAction::EXIT;
		r->status = Status::FAILED;
		return;
	}

	r->action = sAction::RUNNING;
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

	DarkForces::Component::InfElevator* elevator = static_cast<DarkForces::Component::InfElevator* >(m_tree->blackboard("wait_elevator"));
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
