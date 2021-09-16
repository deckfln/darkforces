#include "dfMoveEnemyTo.h"

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaBehaviorTree.h"

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
BehaviorNode* DarkForces::Behavior::MoveEnemyTo::dispatchMessage(gaMessage* message)
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

	return nextNode();
}


GameEngine::BehaviorNode* DarkForces::Behavior::MoveEnemyTo::nextNode(void)
{
	GameEngine::BehaviorNode::Status childStatus;
	
	if (m_status != Status::RUNNING) {
		return this;
	}

	if (m_runningChild >= 0) {
		childStatus = m_children[m_runningChild]->status();
	}

	switch (m_runningChild) {
	case Child::init:
		return startChild(Child::move_to, &m_destination);

	case Child::move_to: {
		if (childStatus == GameEngine::BehaviorNode::Status::SUCCESSED) {
			// we reached the destination
			return exitChild(Status::SUCCESSED);
		}

		// the move_to node failed, it probably collided with something
		gaEntity* collided = static_cast<gaEntity*>(m_tree->blackboard("lastCollision"));
		DarkForces::Component::InfElevator* elevator = static_cast<DarkForces::Component::InfElevator*>(collided->findComponent(DF_COMPONENT_INF_ELEVATOR));

		if (elevator) {
			m_tree->blackboard("wait_elevator", elevator);

			// so we are colliding with an elevator, check the status of the elevator
			switch (elevator->status()) {
			case Component::InfElevator::Status::MOVE:
			case Component::InfElevator::Status::WAIT:
				// wait for the elevator to finish its move, maybe it is opening
				return startChild(Child::wait_door, nullptr);

			case Component::InfElevator::Status::HOLD:
				return startChild(Child::open_door, nullptr);

			case Component::InfElevator::Status::TERMINATED:
				// the elevator will not move any more, so we have to cancel the move
				// whoever asked me to move will notice this is the end
				return exitChild(Status::FAILED);
			}
		}
		else {
			// we collided with a wall 
			return exitChild(Status::FAILED);
		}
		break; }

	case wait_door:
		if (childStatus == Status::SUCCESSED) {
			// now that the door finished moving, restart the move
			return startChild(Child::move_to, &m_destination);
		}
		return exitChild(Status::FAILED);

	case open_door:
		if (childStatus == Status::SUCCESSED) {
			// now that the door is open, restart the move
			return startChild(Child::move_to, &m_destination);
		}
		return exitChild(Status::FAILED);
	}

	return this;
}
