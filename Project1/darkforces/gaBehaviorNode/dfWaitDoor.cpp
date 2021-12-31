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
	return new DarkForces::Behavior::WaitDoor(name);
}


/**
 *
 */
void DarkForces::Behavior::WaitDoor::init(void* data)
{
	m_elevator = static_cast<Component::InfElevator*>(data);
	m_entity->sendDelayedMessage(gaMessage::Action::TICK);

	BehaviorNode::init(data);
}

/**
 *
 */
void DarkForces::Behavior::WaitDoor::dispatchMessage(gaMessage* message, Action* r)
{
	if (message->m_action == gaMessage::Action::TICK) {
		switch (m_elevator->status()) {
		case Component::InfElevator::Status::MOVE:
			// wait for the elevator to finish its move, maybe it is opening
			m_entity->sendDelayedMessage(gaMessage::Action::TICK);
			break;

		case Component::InfElevator::Status::WAIT:
		case Component::InfElevator::Status::HOLD:
			m_status = Status::SUCCESSED;
			m_tree->blackboard<glm::vec3*>("wait_elevator", nullptr);
			break;
		}
	}

	execute(r);
}