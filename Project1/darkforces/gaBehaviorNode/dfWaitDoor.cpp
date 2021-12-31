#include "dfWaitDoor.h"

#include <tinyxml2.h>

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaBehaviorTree.h"

#include "../dfComponent/InfElevator.h"

DarkForces::Behavior::WaitDoor::WaitDoor(const char* name):
	GameEngine::BehaviorNode(name)
{
}

GameEngine::BehaviorNode* DarkForces::Behavior::WaitDoor::clone(GameEngine::BehaviorNode* p)
{
	DarkForces::Behavior::WaitDoor* cl;
	if (p) {
		cl = dynamic_cast<DarkForces::Behavior::WaitDoor*>(p);
	}
	else {
		cl = new DarkForces::Behavior::WaitDoor(m_name);
	}
	return cl;
}

BehaviorNode* DarkForces::Behavior::WaitDoor::create(const char* name, tinyxml2::XMLElement* element, GameEngine::BehaviorNode* used)
{
	DarkForces::Behavior::WaitDoor* node;

	if (used == nullptr) {
		node = new DarkForces::Behavior::WaitDoor(name);
	}
	else {
		node = dynamic_cast<DarkForces::Behavior::WaitDoor*>(used);
	}
	GameEngine::BehaviorNode::create(name, element, node);
	return node;
}

/**
 *
 */
void DarkForces::Behavior::WaitDoor::init(void* data)
{
	m_entity->sendDelayedMessage(gaMessage::Action::TICK);

	BehaviorNode::init(data);
}

/**
 *
 */
void DarkForces::Behavior::WaitDoor::dispatchMessage(gaMessage* message, Action* r)
{
	DarkForces::Component::InfElevator* m_elevator = m_tree->blackboard<DarkForces::Component::InfElevator*>("wait_elevator");

	if (message->m_action == gaMessage::Action::TICK) {
		switch (m_elevator->status()) {
		case Component::InfElevator::Status::MOVE:
			// wait for the elevator to finish its move, maybe it is opening
			m_entity->sendDelayedMessage(gaMessage::Action::TICK);
			break;

		case Component::InfElevator::Status::WAIT:
		case Component::InfElevator::Status::HOLD:
			m_status = Status::SUCCESSED;
			m_tree->blackboard<DarkForces::Component::InfElevator*>("wait_elevator", nullptr);
			break;
		}
	}

	execute(r);
}