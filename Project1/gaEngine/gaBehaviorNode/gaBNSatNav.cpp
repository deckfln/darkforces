#include "gaBNSatNav.h"

#include <map>
#include <imgui.h>

#include "../gaEntity.h"
#include "../gaNavMesh.h"
#include "../World.h"
#include "../gaComponent/gaBehaviorTree.h"

#include "../flightRecorder/frPathFinding.h"

GameEngine::Behavior::SatNav::SatNav(void) :
	GameEngine::BehaviorNode("SatNave")
{
}

/**
 * create the component
 */
GameEngine::Behavior::SatNav::SatNav(const char *name) :
	GameEngine::BehaviorNode(name)
{
	m_previous.resize(16);
}

GameEngine::Behavior::SatNav::SatNav(const char *name, float speed) :
	GameEngine::BehaviorNode(name),
	m_speed(speed)
{
}

/**
 *
 */
void GameEngine::Behavior::SatNav::init(void *data)
{
	glm::vec3* destination = static_cast<glm::vec3*>(data);

	if (m_transforms == nullptr) {
		m_transforms = m_entity->pTransform();
	}

	m_destination = *destination;

	m_navpoints.clear();

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
		BehaviorNode::m_status = BehaviorNode::Status::RUNNING;

		// broadcast the beginning of the move (for animation)
		m_entity->sendDelayedMessage(gaMessage::START_MOVE);
	}
	else {
		// inform everyone of the failure
		BehaviorNode::m_status = BehaviorNode::Status::FAILED;
		m_entity->sendMessage(gaMessage::SatNav_NOGO);
	}
}

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
	if (message->m_extra == nullptr) {
		m_destination = message->m_v3value;
	}
	else {
		m_destination = *(static_cast<glm::vec3*>(message->m_extra));
	}

	m_tree->blackboard("final_target", &m_destination);

	init(&m_destination);
}

/**
 *
 */
void GameEngine::Behavior::SatNav::onMove(gaMessage* message)
{
	if (m_status == Status::MOVE_TO_NEXT_WAYPOINT) {
		// record the current position (to be able to backtrack)
		m_previous[m_previous_current] = m_entity->position();
		m_previous_current++;

		if (m_previous_size < m_previous.size()) {
			m_previous_size++;
		}

		if (m_previous_current >= m_previous.size()) {
			m_previous_current = 0;
		}

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

				BehaviorNode::m_status = BehaviorNode::Status::SUCCESSED;
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
		if (m_currentNavPoint > 0 && m_previous_size > 0) {
			// maybe the next waypoint is next to a wall and we are nearly there, 
			// so backtrack to the previous position and switch to the next waypoint
			glm::vec3 direction;

			if (m_previous_current > 0) {
				m_previous_current--;
			}
			else {
				m_previous_current = m_previous.size() - 1;
			}
			m_previous_size--;

			m_transforms->m_position = m_previous[m_previous_current];
			m_entity->translate(m_transforms->m_position);

			direction = nextWayPoint(true);

			m_transforms->m_position += direction;
			triggerMove(direction);
		}
		else {
			// broadcast the end of the move
			m_status = Status::STILL;
			m_entity->sendInternalMessage(gaMessage::END_MOVE);

			distance = glm::distance(m_entity->position(), m_destination);

			// check how far we are from the destination
			if (distance < m_entity->radius() * 1.5f) {
				BehaviorNode::m_status = BehaviorNode::Status::SUCCESSED;
			}
			else {
				// we are blocked and jumped over all waypoints
				m_tree->blackboard("lastCollision", message->m_pServer);
				BehaviorNode::m_status = BehaviorNode::Status::FAILED;
			}
		}
	}
	else {
		// Move back one step and wait for next turn to retry
		if (m_previous_current > 0) {
			m_previous_current--;
		}
		else {
			m_previous_current = m_previous.size() - 1;
		}

		m_previous_size--;

		m_transforms->m_position = m_previous[m_previous_current];

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
 * let a component deal with a situation
 */
BehaviorNode* GameEngine::Behavior::SatNav::dispatchMessage(gaMessage* message)
{
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

	return nextNode();
}

/**
 * flight recorder
 */
uint32_t GameEngine::Behavior::SatNav::recordState(void* record)
{
	FlightRecorder::SatNav* r = static_cast<FlightRecorder::SatNav*>(record);

	BehaviorNode::recordState(record);

	uint32_t len = sizeof(FlightRecorder::SatNav);
	r->status = static_cast<uint32_t>(m_status);
	r->destination = m_destination;
	r->nbNavPoints = m_navpoints.size();
	r->nbPrevious = m_previous.size();
	r->current = m_currentNavPoint;

	gaEntity* collided = static_cast<gaEntity*>(m_tree->blackboard("lastCollision"));
	if (collided) {
		strncpy_s(r->lastCollision, sizeof(r->lastCollision), collided->name().c_str(), _TRUNCATE);
	}
	else {
		r->lastCollision[0] = 0;
	}

	if (r->nbNavPoints + r->nbPrevious > 1023) {
		__debugbreak();
	}

	uint32_t i = 0;
	for (uint32_t j = i; j < m_navpoints.size(); j++) {
		r->points[i++] = m_navpoints[j];
		len += sizeof(glm::vec3);
	}
	for (uint32_t j = i; j < m_previous.size(); j++) {
		r->points[i++] = m_previous[j];
		len += sizeof(glm::vec3);
	}

	r->node.size = len;
	return len;
}

uint32_t GameEngine::Behavior::SatNav::loadState(void* record)
{
	BehaviorNode::loadState(record);

	FlightRecorder::SatNav* r = static_cast<FlightRecorder::SatNav*>(record);

	m_status = static_cast<Status>(r->status);
	m_destination = r->destination;
	m_currentNavPoint = r->current;

	if (r->lastCollision[0] != 0) {
		gaEntity* collided = g_gaWorld.getEntity(r->lastCollision);
		m_tree->blackboard("lastCollision", collided);
	}

	m_navpoints.resize(r->nbNavPoints);
	m_previous.resize(r->nbPrevious);

	uint32_t i = 0;
	for (uint32_t j = 0; j < r->nbNavPoints; j++) {
		m_navpoints[j] = r->points[i++];
	}
	for (uint32_t j = 0; j < r->nbPrevious; j++) {
		m_previous[j] = r->points[i++];
	}

	return r->node.size;
}
