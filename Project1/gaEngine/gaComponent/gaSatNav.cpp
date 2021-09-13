#include "gaSatNav.h"

#include <map>
#include <imgui.h>

#include "../gaEntity.h"
#include "../gaNavMesh.h"
#include "../flightRecorder/frPathFinding.h"

const char* g_className = "SatNav";

/**
 * return the direction to the next way point
 */
glm::vec3 GameEngine::Component::SatNav::nextWayPoint(bool normalize)
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
void GameEngine::Component::SatNav::triggerMove(void)
{
	m_transforms->m_flag = gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL;
	m_entity->sendDelayedMessage(
		gaMessage::WANT_TO_MOVE,
		gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL,
		m_transforms);
}

void GameEngine::Component::SatNav::triggerMove(const glm::vec3& direction)
{
	triggerMove();

	// turn the entity in the direction of the move
	m_entity->sendInternalMessage(
		gaMessage::LOOK_AT,
		-direction);
}

/**
 * create the component
 */
GameEngine::Component::SatNav::SatNav(float speed) :
	gaComponent(gaComponent::PathFinding),
	m_speed(speed)
{
}

/**
 * let a component deal with a situation
 */
void GameEngine::Component::SatNav::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::WORLD_INSERT:
		m_transforms = m_entity->pTransform();
		break;

	case gaMessage::Action::SatNav_GOTO:
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
		break;

	case gaMessage::Action::SatNav_CANCEL:
		m_navpoints.clear();
		m_status = Status::STILL;

		// broadcast the end of the move (for animation)
		m_entity->sendInternalMessage(gaMessage::END_MOVE);
		break;

	case gaMessage::Action::MOVE:
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
		break;

	case gaMessage::Action::COLLIDE: {
		float distance = glm::length(m_entity->position() - m_transforms->m_position);

		if (distance < m_entity->radius() * 1.5f) {
			if (m_currentNavPoint > 0) {
				// maybe the next waypoint is next to a wall and we are nearly there, 
				// so backtrack to the previous position and switch to the next waypoint
				glm::vec3 p;
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
			}
		}
		else {
			// Move back one step and wait for next turn to retry
			m_transforms->m_position = m_previous.back();
			m_previous.pop_back();
			triggerMove();
		}
		break; }

	case gaMessage::Action::WOULD_FALL:
		// find an other path
		__debugbreak();
		break;
	}
}

/**
 * size of the component record
 */
inline uint32_t GameEngine::Component::SatNav::recordSize(void)
{
	return sizeof(GameEngine::flightRecorder::PathFinding);
}

/**
 * save the component state in a record
 */
uint32_t GameEngine::Component::SatNav::recordState(void* record)
{
	GameEngine::flightRecorder::PathFinding* r = static_cast<GameEngine::flightRecorder::PathFinding*>(record);

	r->size = sizeof(GameEngine::flightRecorder::PathFinding);
	r->m_status = static_cast<uint32_t>(m_status);
	r->m_destination = m_destination;
	r->m_speed = m_speed;
	r->m_currentNavPoint = m_currentNavPoint;					// beware, backtrack as navpoints a	re in reverse order
	r->c_navpoints = m_navpoints.size();
	for (uint32_t i = 0; i < m_navpoints.size(); i++) {
		r->m_navpoints[i] = m_navpoints[i];
	}
	r->c_previous = m_previous.size();
	for (uint32_t i = 0; i < m_previous.size(); i++) {
		r->m_previous[i] = m_previous[i];
	}
	return r->size;
}

/**
 * reload a component state from a record
 */
uint32_t GameEngine::Component::SatNav::loadState(void* record)
{
	GameEngine::flightRecorder::PathFinding* r = static_cast<GameEngine::flightRecorder::PathFinding*>(record);

	m_status = static_cast<SatNav::Status>(r->m_status);
	m_destination = r->m_destination;
	m_speed = r->m_speed;
	m_currentNavPoint = r->m_currentNavPoint;					// beware, backtrack as navpoints a	re in reverse order
	m_navpoints.resize(r->c_navpoints);
	for (uint32_t i = 0; i < m_navpoints.size(); i++) {
		m_navpoints[i] = r->m_navpoints[i];
	}
	m_previous.resize(r->c_previous);
	for (uint32_t i = 0; i < m_previous.size(); i++) {
		m_previous[i] = r->m_previous[i];
	}
	return sizeof(GameEngine::flightRecorder::PathFinding);
}

/**
 * debugger
 */
void GameEngine::Component::SatNav::debugGUIinline(void)
{
	static std::map<Status, const char*> g_display = {
		{ Status::STILL, "Still"},
		{ Status::MOVE_TO_NEXT_WAYPOINT, "Move 2 next WP"},
		{ Status::REACHED_NEXT_WAYPOINT, "Reached Next WP"},
		{ Status::NEARLY_REACHED_NEXT_WAYPOINT, "Nearly Reached Next WP"}
	};

	if (ImGui::TreeNode(g_className)) {
		ImGui::Text("Status:%s", g_display[m_status]);
		ImGui::Text("Target: %.2f %.2f %.2f", m_destination.x, m_destination.y, m_destination.z);
		ImGui::Text("Speed: %.2f", m_speed);
		if (ImGui::TreeNode("Way Points")) {
			for (int32_t i = m_navpoints.size() - 1; i >= 0; i--) {
				ImGui::Text("%s%.2f %.2f %.2f", 
					(i==m_currentNavPoint) ? ">" : " ",
					m_navpoints[i].x, 
					m_navpoints[i].y,
					m_navpoints[i].z);
			}
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
}