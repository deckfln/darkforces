#include "gaMoveTo.h"

#include <map>
#include <imgui.h>

#include "../gaEntity.h"
#include "../gaNavMesh.h"
#include "../World.h"
#include "../gaComponent/gaBehaviorTree.h"

#include "../flightRecorder/frPathFinding.h"

GameEngine::Behavior::MoveTo::MoveTo(void) :
	GameEngine::BehaviorNode("SatNave")
{
}

/**
 * create the component
 */
GameEngine::Behavior::MoveTo::MoveTo(const char *name) :
	GameEngine::BehaviorNode(name)
{
	m_previous.resize(16);
}

GameEngine::Behavior::MoveTo::MoveTo(const char *name, float speed) :
	GameEngine::BehaviorNode(name),
	m_speed(speed)
{
}

/**
 *
 */
void GameEngine::Behavior::MoveTo::init(void *data)
{
	m_navpoints = static_cast<std::vector<glm::vec3>*>(data);

	if (m_transforms == nullptr) {
		m_transforms = m_entity->pTransform();
	}

	// there is a path
	m_currentNavPoint = m_navpoints->size() - 1;

	// trigger the move
	m_speed = 0.035f;
	m_transforms->m_position = m_entity->position(); //m_navpoints->at(m_currentNavPoint);
	m_entity->sendDelayedMessage(
		gaMessage::WANT_TO_MOVE,
		gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL,
		m_transforms);

	m_status = Status::MOVE_TO_NEXT_WAYPOINT;
	BehaviorNode::m_status = BehaviorNode::Status::RUNNING;

	// broadcast the beginning of the move (for animation)
	m_entity->sendDelayedMessage(gaMessage::START_MOVE);
}

/**
 * return the direction to the next way point
 */
glm::vec3 GameEngine::Behavior::MoveTo::nextWayPoint(bool normalize)
{
	m_currentNavPoint--;
	glm::vec3 p = m_navpoints->at(m_currentNavPoint);
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
void GameEngine::Behavior::MoveTo::triggerMove(void)
{
	m_transforms->m_flag = gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL;
	m_entity->sendDelayedMessage(
		gaMessage::WANT_TO_MOVE,
		gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL,
		m_transforms);
}

void GameEngine::Behavior::MoveTo::triggerMove(const glm::vec3& direction)
{
	triggerMove();

}

/**
 * select the next waypoint or end the movement
 */
void GameEngine::Behavior::MoveTo::onReachedNextWayPoint(gaMessage *message)
{
	glm::vec3 direction;

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

	if (m_status == Status::MOVE_TO_NEXT_WAYPOINT) {
		m_transforms->m_position = m_entity->position() + direction;
		triggerMove(direction);
	}
}

/**
 * cancel the satnav because there is a obstable in front
 */
void GameEngine::Behavior::MoveTo::onBlockedWay(gaMessage *message)
{
	// broadcast the end of the move
	m_status = Status::STILL;
	m_entity->sendInternalMessage(gaMessage::END_MOVE);

	float distance = glm::distance(m_entity->position(), m_destination);

	// check how far we are from the destination
	if (distance < m_entity->radius() * 1.5f) {
		BehaviorNode::m_status = BehaviorNode::Status::SUCCESSED;
	}
	else {
		// we are blocked and jumped over all waypoints
		m_tree->blackboard("lastCollision", &message->m_data);
		BehaviorNode::m_status = BehaviorNode::Status::FAILED;
	}
}

/**
 * monitor the entity moving
 */
void GameEngine::Behavior::MoveTo::onMove(gaMessage* message)
{
	if (m_status != Status::MOVE_TO_NEXT_WAYPOINT) {
		// ignore the message if there is no navpoint programmed
		return;
	}

	// record the current position (to be able to backtrack)
	m_previous[m_previous_current] = m_entity->position();
	m_previous_current++;

	if (m_previous_size < m_previous.size()) {
		m_previous_size++;
	}

	if (m_previous_current >= m_previous.size()) {
		m_previous_current = 0;
	}

	// how far from the navpoint
	glm::vec3 p = m_navpoints->at(m_currentNavPoint);
	glm::vec3 direction = p - m_entity->position();
	direction.y = 0;	// move forward, physics will take care of problems

	float d = glm::length(direction);

	// turn the entity in the direction of the move
	glm::vec3 lookAt = glm::normalize(direction);
	if (d < m_entity->radius() * 2.0f && m_currentNavPoint > 1) {
		// interpolate the direction with the next waypoint if we are near the current waypoint and it is not the last one
		glm::vec3 next_direction = glm::normalize(m_navpoints->at(m_currentNavPoint - 1) - m_navpoints->at(m_currentNavPoint));
		glm::vec3 current_direction = lookAt;

		float inter = d / (m_entity->radius() * 2.0f);
		lookAt = current_direction * inter + next_direction * (1.0f-inter);
	}

	lookAt = glm::normalize(lookAt);
	m_entity->sendInternalMessage(
			gaMessage::LOOK_AT,
			-lookAt);

	/*
	printf("%.04f,%.04f,%.04f,%.04f,%.04f,%.04f,%.04f,%.04f,\n", 
		m_entity->position().x, m_entity->position().z, 
		current_direction.x, current_direction.z,
		next_direction.x, next_direction.z,
		lookAt.x, lookAt.z);
	*/

	// did we reach the next navpoint ?
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
		return onReachedNextWayPoint(message);
	}

	// and take action
	if (m_status == Status::MOVE_TO_NEXT_WAYPOINT) {
		m_transforms->m_position = m_entity->position() + direction;
		triggerMove(direction);
	}
}

/**
 *
 */
void GameEngine::Behavior::MoveTo::onCollide(gaMessage* message)
{
	float radius = m_entity->radius();
	glm::vec3 position = m_entity->position();
	glm::vec3 failedPosition = m_transforms->m_position;
	float distance = glm::length(position - failedPosition);

	// check if we are not stuck, test the last 3 positions
	if (m_previous.size() > 0) {
		int32_t j;
		uint32_t count = 0;
		glm::vec2 barycenter(0);

		for (uint32_t i = 0; i < 4; i++) {
			j = m_previous_current - i;

			// Move back one step and wait for next turn to retry
			if (j < 0) {
				j = m_previous.size() + j;
			}

			barycenter.x += m_previous[j].x;
			barycenter.y += m_previous[j].z;
		}
		barycenter /= 4.0f;
		float move_radius = 0;
		for (uint32_t i = 0; i < 4; i++) {
			j = m_previous_current - i;

			// Move back one step and wait for next turn to retry
			if (j < 0) {
				j = m_previous.size() + j;
			}

			glm::vec2 p(m_previous[j].x, m_previous[j].z);
			move_radius += glm::distance(p, barycenter);
		}
		move_radius /= 4.0f;

		if (move_radius < radius / 2.0f) {
			// give up, we are facing a not planned object that refuses to move
			return onBlockedWay(message);
		}
	}

	// maybe the next waypoint is next to a wall and we are nearly there, 
	// so backtrack to the previous position, move to the opposite direction of the collision and continue
	glm::vec3 collision = message->m_v3value;
	glm::vec3 target = m_navpoints->at(m_currentNavPoint);

	// distance between the failed position and the navpoint
	glm::vec2 t(target.x, target.z);
	glm::vec2 f(failedPosition.x, failedPosition.z);
	float d = glm::length(t - f);
	if (d < radius) {
		// the waypoint is unreachable, so jump over and move to the next one
		m_status = Status::REACHED_NEXT_WAYPOINT;
		return onReachedNextWayPoint(message);
	}

	glm::vec2 a(position.x, position.z);
	glm::vec2 b(failedPosition.x, failedPosition.z);
	glm::vec2 c(collision.x, collision.z);

	glm::vec2 ab(b - a);
	glm::vec2 ac(c - a);

	d = glm::dot(ac, ab) / glm::length2(ab);
	glm::vec2 p = a + ab * d;			// project collision on (position, target)
	glm::vec2 o = p + (p - c);			// project collision on the other side of (position, target)
	glm::vec2 ao = glm::normalize(o - a) * glm::length(ab);
	glm::vec2 new_target = a + ao;

	// ensure we are at least radius from the original collision
	if (glm::distance(new_target, c) < radius) {
		glm::vec2 nc(glm::normalize(new_target - c) * radius);
		new_target = c + nc;
	}

	m_transforms->m_position = glm::vec3(new_target.x, position.y, new_target.y);
	triggerMove(m_transforms->m_position - position);

	//printf("%.04f,%.04f,%.04f,%.04f,%.04f,%.04f,%.04f,%.04f,\n", a.x, a.y, b.x, b.y, new_target.x, new_target.y, c.x, c.y);
}

/**
 * cancel the move
 */
void GameEngine::Behavior::MoveTo::onCancel(gaMessage* message)
{
	m_status = Status::STILL;

	// broadcast the end of the move (for animation)
	m_entity->sendInternalMessage(gaMessage::END_MOVE);

	BehaviorNode::m_status = BehaviorNode::Status::SUCCESSED;
}

/**
 * let a component deal with a situation
 */
void GameEngine::Behavior::MoveTo::dispatchMessage(gaMessage* message, Action *r)
{
	switch (message->m_action) {
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

	return execute(r);
}

/**
 * flight recorder
 */
uint32_t GameEngine::Behavior::MoveTo::recordState(void* record)
{
	FlightRecorder::MoveTo* r = static_cast<FlightRecorder::MoveTo*>(record);

	BehaviorNode::recordState(record);

	uint32_t len = sizeof(FlightRecorder::MoveTo);
	r->status = static_cast<uint32_t>(m_status);
	r->nbPrevious = m_previous.size();
	r->current = m_currentNavPoint;

	gaEntity* collided = m_tree->blackboard<gaEntity>("lastCollision");
	if (collided) {
		strncpy_s(r->lastCollision, sizeof(r->lastCollision), collided->name().c_str(), _TRUNCATE);
	}
	else {
		r->lastCollision[0] = 0;
	}

	if (r->nbPrevious > 1023) {
		__debugbreak();
	}

	/*
	uint32_t i = 0;
	for (uint32_t j = i; j < m_previous.size(); j++) {
		r->points[i++] = m_previous[j];
		len += sizeof(glm::vec3);
	}
	*/

	r->node.size = len;
	return len;
}

uint32_t GameEngine::Behavior::MoveTo::loadState(void* record)
{
	BehaviorNode::loadState(record);

	FlightRecorder::MoveTo* r = static_cast<FlightRecorder::MoveTo*>(record);

	m_status = static_cast<Status>(r->status);
	m_currentNavPoint = r->current;

	if (r->lastCollision[0] != 0) {
		gaEntity* collided = g_gaWorld.getEntity(r->lastCollision);
		m_tree->blackboard<gaEntity>("lastCollision", collided);
	}

	m_previous.resize(r->nbPrevious);

	/*
	uint32_t i = 0;
	for (uint32_t j = 0; j < r->nbPrevious; j++) {
		m_previous[j] = r->points[i++];
	}
	*/

	return r->node.size;
}
