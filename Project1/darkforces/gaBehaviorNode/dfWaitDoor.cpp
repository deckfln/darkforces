#include "dfWaitDoor.h"

#include <tinyxml2.h>
#include <imgui.h>

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/gaComponent/gaBehaviorTree.h"

#include "../dfComponent/InfElevator.h"

static const char* g_className = "DarkForces:waitDoor";

DarkForces::Behavior::WaitDoor::WaitDoor(const char* name):
	GameEngine::BehaviorNode(name)
{
	m_className = g_className;
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
 * if the elevator is moving, wait for the opening. Else fail.
 */
void DarkForces::Behavior::WaitDoor::init(void* data)
{
	DarkForces::Component::InfElevator* m_elevator = m_tree->blackboard().pGet<DarkForces::Component::InfElevator>("wait_elevator", GameEngine::Variable::Type::PTR);

	switch (m_elevator->status()) {
	case Component::InfElevator::Status::MOVE:
		// wait for the elevator to finish its move, maybe it is opening
		m_entity->sendDelayedMessage(gaMessage::Action::TICK);
		break;

	case Component::InfElevator::Status::WAIT:
	case Component::InfElevator::Status::HOLD:
		m_status = Status::FAILED;
		return;
	}

	BehaviorNode::init(data);
}

/**
 *
 */
void DarkForces::Behavior::WaitDoor::dispatchMessage(gaMessage* message, Action* r)
{
	DarkForces::Component::InfElevator* m_elevator = m_tree->blackboard().pGet<DarkForces::Component::InfElevator>("wait_elevator", GameEngine::Variable::Type::PTR);

	if (message->m_action == gaMessage::Action::TICK) {
		switch (m_elevator->status()) {
		case Component::InfElevator::Status::MOVE:
			// wait for the elevator to finish its move, maybe it is opening
			m_entity->sendDelayedMessage(gaMessage::Action::TICK);
			break;

		case Component::InfElevator::Status::WAIT:
		case Component::InfElevator::Status::HOLD:
			return succeeded(r);
			break;
		}
	}

	execute(r);
}

//---------------------------------------

/**
 * Debugger
 */
void DarkForces::Behavior::WaitDoor::debugGUInode(void)
{
	DarkForces::Component::InfElevator* elevator = m_tree->blackboard().pGet<DarkForces::Component::InfElevator>("wait_elevator", GameEngine::Variable::Type::PTR);
	if (elevator != nullptr) {
		gaEntity* entity = elevator->entity();
		ImGui::Text("Wait elevator: %s", entity->name().c_str());
	}
}

