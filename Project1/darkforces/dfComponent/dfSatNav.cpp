#include "dfSatNav.h"

#include "../../gaEngine/gaEntity.h"
#include "../../gaEngine/World.h"
#include "../../gaEngine/gaNavMesh.h"

#include "InfElevator.h"
#include "../dfLogicTrigger.h"
#include "../dfSector.h"
#include "../../config.h"

/**
 * parse the DF sector and sort the triggers
 * the nearest is more likely to be the correct one
 */
static dfLogicTrigger* findTrigger(const glm::vec3& from, gaEntity* entity)
{
	dfSector* sector = dynamic_cast<dfSector*>(entity);

	const std::list<dfLogicTrigger*>& triggers = sector->triggers();

	float l, len = 9999999999.0f;
	dfLogicTrigger* target = nullptr;

	std::vector<glm::vec3> m_navpoints;

	for (auto trigger : triggers) {
		m_navpoints.clear();
		l = g_navMesh.findPath(from, trigger->position(), m_navpoints);
		if (l < len) {
			len = l;
			target = trigger;
		}
	}

	return target;
}

/**
 * Set the destination to the next trigger that can activate an elevator
 */
void DarkForces::Component::SatNav::goto_next_trigger(void)
{
	switch (m_status) {
	case Status::GOTO_FIRST_TRIGGER:
		m_status = Status::GOTO_NEXT_TRIGGER;
		break;
	default:
		m_status = Status::GOTO_FIRST_TRIGGER;
	}

	if (m_triggers.size() > 0) {
		m_targetTrigger = m_triggers.front();
		m_triggers.pop_front();

		// and go to the first trigger
		m_entity->sendInternalMessage(gaMessage::Action::SatNav_GOTO, m_targetTrigger->position());
	}
	else {
		// so we tried all triggers and still cant open the door => give up
		m_status = Status::NOGO;
		m_entity->sendInternalMessage(gaMessage::Action::SatNav_NOGO);
		m_targetTrigger = nullptr;
	}
}

/**
 * activate the current targeted trigger
 */
void DarkForces::Component::SatNav::activate_trigger(void)
{
	// activate the trigger
	m_targetTrigger->activate(m_entity->name());
	m_status = Status::WAIT_DOOR;
	m_entity->sendDelayedMessage(DarkForces::Message::SatNav_Wait);

	// broadcast the end of the move (for animation)
	m_entity->sendInternalMessage(gaMessage::END_MOVE);

	m_triggers.clear();
	m_targetTrigger = nullptr;
}

/**
 * init a move to a destination
 */
bool DarkForces::Component::SatNav::onSatNav_goto(gaMessage* message)
{
	switch (m_status) {
	case Status::NOGO:
	case Status::STILL:
	case Status::REACHED_DESTINATION:
		m_status = Status::MOVE_TO_DESTINATION;
		break;
	case Status::MOVE_TO_DESTINATION:
		break;
	case Status::GOTO_FIRST_TRIGGER:
		m_destinations.push_back(m_currentDestination);
		break;
	case Status::GOTO_NEXT_TRIGGER:
		break;
	}

	glm::vec3 p;
	if (message->m_extra == nullptr) {
		p = message->m_v3value;
	}
	else {
		p = *(static_cast<glm::vec3*>(message->m_extra));
	}

	m_currentDestination = p;

	return false;
}

/**
 * deal with collide message
 */
bool DarkForces::Component::SatNav::onCollide(gaMessage* message)
{
	gaEntity* entity = message->m_pServer;
	DarkForces::Component::InfElevator* elevator = static_cast<DarkForces::Component::InfElevator*>(entity->findComponent(DF_COMPONENT_INF_ELEVATOR));
	dfLogicTrigger* trigger = dynamic_cast<dfLogicTrigger*>(entity);

	// are we reaching a trigger we were looking for
	switch (m_status) {
	case Status::GOTO_FIRST_TRIGGER:
	case Status::GOTO_NEXT_TRIGGER:
		if (m_targetTrigger == trigger) {
			activate_trigger();
		}
		else {
			// we hit "something" let's test the distance from here to the trigger. if we are near, let's pretend everything is OK
			float d = glm::distance(m_entity->position(), m_targetTrigger->position());

			if (d < 0.5f) {
				activate_trigger();
			}
			else {
				goto_next_trigger();
			}
		}
		return true;	// do not pass the message to GameEngine::SatNav

	default:
		if (elevator) {
			// so we are colliding with an elevator, check the status of the elevator
			switch (elevator->status()) {
			case InfElevator::Status::MOVE:
			case InfElevator::Status::WAIT:
				// wait for the elevator to finish its move, maybe it is opening
				m_status = Status::WAIT_DOOR;
				m_entity->sendDelayedMessage(DarkForces::Message::SatNav_Wait);
				break;

			case InfElevator::Status::HOLD: {
				if (m_status == Status::GOTO_NEXT_TRIGGER) {
					// if we are looking for a trigger, this means we cant reach the trigger, so we need to test the next one
					goto_next_trigger();
				}
				else {
					// we are on a natural move, to the elevator can be activated
					// test all triggers of the object
					dfSector* sector = dynamic_cast<dfSector*>(entity);
					m_triggers = sector->triggers();

					goto_next_trigger();

					// register the elevator we want to open
					m_nextElevator.push_back(elevator);
				}
				break;  };

			case InfElevator::Status::TERMINATED:
				// the elevator will not move any more, so we have to cancel the move
				// whoever asked me to move will notice this is the end
				m_entity->sendInternalMessage(gaMessage::Action::SatNav_CANCEL);
				m_entity->sendInternalMessage(gaMessage::Action::SatNav_NOGO);
				break;
			}
			return true;	// do not pass the message to GameEngine::SatNav
		}
	}

	// pass the message to GameEngine::SatNav
	return false;
}

/**
 * wait for a door to open
 */
bool DarkForces::Component::SatNav::onSatNav_wait(gaMessage* message)
{
	DarkForces::Component::InfElevator* elevator = m_nextElevator.back();

	switch (elevator->status()) {
	case InfElevator::Status::MOVE:
		// wait for the elevator to finish its move, maybe it is opening
		m_entity->sendDelayedMessage(DarkForces::Message::SatNav_Wait);
		break;

	case InfElevator::Status::WAIT:
	case InfElevator::Status::HOLD:
		m_currentDestination = m_destinations.back();
		m_destinations.pop_back();

		// the door is open, go to the destination
		m_status = Status::MOVE_TO_DESTINATION;
		m_entity->sendInternalMessage(gaMessage::Action::SatNav_GOTO, m_currentDestination);
		break;
	}

	return false;
}

/**
 * satnav reached its destination
 */
bool DarkForces::Component::SatNav::onSatNav_Reached(gaMessage* message)
{
	if (!m_destinations.empty()) {
		glm::vec3 p = m_destinations.back();
		m_destinations.pop_back();

		m_entity->sendInternalMessage(gaMessage::Action::SatNav_GOTO, p);
	}
	else {
		m_status = Status::REACHED_DESTINATION;
	}

	return false;
}

/**
 * create the callback function
 */
DarkForces::Component::SatNav::SatNav(float speed) :
	GameEngine::Component::SatNav(speed)
{
	m_messages = {
		{gaMessage::Action::SatNav_GOTO, &DarkForces::Component::SatNav::onSatNav_goto},
		{gaMessage::Action::SatNav_GOTO, &DarkForces::Component::SatNav::onSatNav_goto},
		{gaMessage::Action::SatNav_REACHED, &DarkForces::Component::SatNav::onSatNav_Reached},
		{gaMessage::Action::COLLIDE, &DarkForces::Component::SatNav::onCollide},
		{DarkForces::Message::SatNav_Wait, &DarkForces::Component::SatNav::onSatNav_wait}
	};
}

/**
 *
 */
void DarkForces::Component::SatNav::dispatchMessage(gaMessage* message)
{
	auto callback = m_messages[message->m_action];

	if (callback) {
		if ((this->*callback)(message)) {
			return;	// do not pass the message down
		}
	}

	GameEngine::Component::SatNav::dispatchMessage(message);
}