#include "dfWaitDoor.h"

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaBehaviorTree.h"

#include "../dfComponent/InfElevator.h"

DarkForces::Behavior::WaitDoor::WaitDoor(const char* name):
	GameEngine::BehaviorNode(name)
{
}

/**
 *
 */
void DarkForces::Behavior::WaitDoor::init(void* data)
{
	m_status = Status::RUNNING;
	m_entity->sendDelayedMessage(gaMessage::Action::TICK);
}

/**
 *
 */
BehaviorNode* DarkForces::Behavior::WaitDoor::dispatchMessage(gaMessage* message)
{
	if (message->m_action == gaMessage::Action::TICK) {
		DarkForces::Component::InfElevator* elevator = static_cast<DarkForces::Component::InfElevator*>(m_tree->blackboard("wait_elevator"));

		switch (elevator->status()) {
		case Component::InfElevator::Status::MOVE:
			// wait for the elevator to finish its move, maybe it is opening
			m_entity->sendDelayedMessage(gaMessage::Action::TICK);
			break;

		case Component::InfElevator::Status::WAIT:
		case Component::InfElevator::Status::HOLD:
			m_status = Status::SUCCESSED;
			break;
		}
	}

	return nextNode();
}

