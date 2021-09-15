#include "gaBNSatNav.h"

#include <map>
#include <imgui.h>

#include "../gaEntity.h"
#include "../gaNavMesh.h"
#include "../flightRecorder/frPathFinding.h"

/**
 * return the direction to the next way point
 */
glm::vec3 GameEngine::Behavior::SatNav::nextWayPoint(bool normalize)
{
	m_currentNavPoint--;
	glm::vec3 p = m_navpoints[m_currentNavPoint];
	glm::vec3 direction = p - m_entity->position();
	direction.y = 0;	// move forward, physics will take care of problems

	if (normalize) {
		direction = glm::normalize(direction) * m_speed;
	}

	m_status = Status::MOVE_TO_NEXT_WAYPOINT;

	return direction;
}

/**
 * send the move messages
 */
void GameEngine::Behavior::SatNav::triggerMove(void)
{
	m_transforms->m_flag = gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL;
	m_entity->sendDelayedMessage(
		gaMessage::WANT_TO_MOVE,
		gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL,
		m_transforms);
}

void GameEngine::Behavior::SatNav::triggerMove(const glm::vec3& direction)
{
	triggerMove();

	// turn the entity in the direction of the move
	m_entity->sendInternalMessage(
		gaMessage::LOOK_AT,
		-direction);
}

/**
 *
 */
void GameEngine::Behavior::SatNav::onGoto(gaMessage* message)
{
	m_navpoints.clear();

	if (message->m_extra == nullptr) {
		m_destination = message->m_v3value;
	}
	else {
		m_destination = *(static_cast<glm::vec3*>(message->m_extra));
	}

	if (g_navMesh.findPath(m_entity->position(), m_destination, m_navpoints) > 0) {
		// there is a path
		m_currentNavPoint = m_navpoints.size() - 1;

		// trigger the move
		m_speed = 0.035f;
		m_transforms->m_position = m_navpoints[m_currentNavPoint];
		m_entity->sendDelayedMessage(
			gaMessage::WANT_TO_MOVE,
			gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL,
			m_transforms);

		m_status = Status::MOVE_TO_NEXT_WAYPOINT;

		// broadcast the beginning of the move (for animation)
		m_entity->sendInternalMessage(gaMessage::START_MOVE);
	}
	else {
		// inform everyone of the failure
		m_entity->sendInternalMessage(gaMessage::SatNav_NOGO);
	}
	BehaviorNode::m_status = BehaviorNode::Status::RUNNING;
}

/**
 *
 */
void GameEngine::Behavior::SatNav::onMove(gaMessage* message)
{
	if (m_status == Status::MOVE_TO_NEXT_WAYPOINT) {
		// record the current position (to be able to backtrack)
		m_previous.push_back(m_entity->position());

		glm::vec3 p = m_navpoints[m_currentNavPoint];

		glm::vec3 direction = p - m_entity->position();
		direction.y = 0;	// move forward, physics will take care of problems

		// did we reach the next navpoint ?
		float d = glm::length(direction);
		if (d < 0.01f) {
			m_status = Status::REACHED_NEXT_WAYPOINT;
		}
		else if (d < m_speed) {
			m_status = Status::NEARLY_REACHED_NEXT_WAYPOINT;
		}
		else {
			// and continue to move forward
			direction = glm::normalize(direction) * m_speed;
		}

		// test the result of the move
		switch (m_status) {
		case Status::NEARLY_REACHED_NEXT_WAYPOINT:
			if (m_currentNavPoint > 0) {
				// move to the next waypoint directly
				direction = nextWayPoint(true);
			}
			else {
				// do the last step
			}
			m_status = Status::MOVE_TO_NEXT_WAYPOINT;
			break;

		case Status::REACHED_NEXT_WAYPOINT:
			if (m_currentNavPoint == 0) {
				m_status = Status::STILL;

				// broadcast the end of the move (for animation)
				m_entity->sendInternalMessage(gaMessage::END_MOVE);

				// broadcast the point reached
				m_entity->sendInternalMessage(gaMessage::SatNav_REACHED);
			}
			else {
				// if we reached the next navpoint, move to the next one
				direction = nextWayPoint(true);
			}
			break;
		}

		// and take action
		if (m_status == Status::MOVE_TO_NEXT_WAYPOINT) {
			m_transforms->m_position = m_entity->position() + direction;
			triggerMove(direction);
		}
	}
}

/**
 *
 */
void GameEngine::Behavior::SatNav::onCollide(gaMessage* message)
{
	float distance = glm::length(m_entity->position() - m_transforms->m_position);

	if (distance < m_entity->radius() * 1.5f) {
		if (m_currentNavPoint > 0) {
			// maybe the next waypoint is next to a wall and we are nearly there, 
			// so backtrack to the previous position and switch to the next waypoint
			glm::vec3 direction;

			m_transforms->m_position = m_previous.back();
			m_entity->translate(m_transforms->m_position);
			m_previous.pop_back();

			direction = nextWayPoint(true);

			m_transforms->m_position += direction;
			triggerMove(direction);
		}
		else {
			// broadcast the end of the move
			m_status = Status::STILL;
			m_entity->sendInternalMessage(gaMessage::END_MOVE);
			BehaviorNode::m_status = BehaviorNode::Status::SUCCESSED;
		}
	}
	else {
		// Move back one step and wait for next turn to retry
		m_transforms->m_position = m_previous.back();
		m_previous.pop_back();
		triggerMove();
	}
}

/**
 *
 */
void GameEngine::Behavior::SatNav::onCancel(gaMessage* message)
{
	m_navpoints.clear();
	m_status = Status::STILL;

	// broadcast the end of the move (for animation)
	m_entity->sendInternalMessage(gaMessage::END_MOVE);

	BehaviorNode::m_status = BehaviorNode::Status::FAILED;
}

/**
 * create the component
 */
GameEngine::Behavior::SatNav::SatNav(void) :
	GameEngine::BehaviorNode()
{
}

GameEngine::Behavior::SatNav::SatNav(float speed) :
	GameEngine::BehaviorNode(),
	m_speed(speed)
{
}

/**
 * let a component deal with a situation
 */
void GameEngine::Behavior::SatNav::dispatchMessage(gaMessage* message)
{
	if (m_transforms == nullptr) {
		m_transforms = m_entity->pTransform();
	}

	switch (message->m_action) {
	case gaMessage::Action::SatNav_GOTO:
		onGoto(message);
		break;

	case gaMessage::Action::SatNav_CANCEL:
		onCancel(message);
		break;

	case gaMessage::Action::MOVE:
		onMove(message);
		break;

	case gaMessage::Action::COLLIDE: 
		onCollide(message);
		break;

	case gaMessage::Action::WOULD_FALL:
		// find an other path
		__debugbreak();
		break;
	}
}
