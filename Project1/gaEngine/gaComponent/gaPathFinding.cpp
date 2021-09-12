#include "gaPathFinding.h"

#include <map>
#include <imgui.h>

#include "../gaEntity.h"
#include "../gaNavMesh.h"

const char* g_className = "PathFinding";

GameEngine::Component::PathFinding::PathFinding(float speed) :
	gaComponent(gaComponent::PathFinding),
	m_speed(speed)
{
}

/**
 * let a component deal with a situation
 */
void GameEngine::Component::PathFinding::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::Action::WORLD_INSERT:
		m_transforms = m_entity->pTransform();

		if (m_entity->name() == "OFFCFIN.WAX(21)") {
			m_status = Status::MOVE_TO_NEXT_WAYPOINT;
			m_destination = glm::vec3(-20.16, -3.0, 18.84);

			if (g_navMesh.findPath(m_entity->position(), m_destination, m_navpoints)) {
				// there is a path
				m_currentNavPoint = m_navpoints.size() - 1;

				m_speed = 0.035f;
				m_transforms->m_position = m_navpoints[m_currentNavPoint];
				m_entity->sendDelayedMessage(
					gaMessage::WANT_TO_MOVE,
					gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL,
					m_transforms);

				// broadcast the beginning of the move
				m_entity->sendInternalMessage(gaMessage::START_MOVE);
			}
		}
		break;

	case gaMessage::Action::MOVE:
		// record the current position (to be able to backtrack)
		m_previous.push_back(m_entity->position());

		if (m_status == Status::MOVE_TO_NEXT_WAYPOINT) {
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
					m_currentNavPoint--;
					p = m_navpoints[m_currentNavPoint];
					direction = p - m_entity->position();
					direction.y = 0;	// move forward, physics will take care of problems
					direction = glm::normalize(direction) * m_speed;
				}
				else {
					// do the last step
				}

				m_status = Status::MOVE_TO_NEXT_WAYPOINT;
				break;

			case Status::REACHED_NEXT_WAYPOINT:
				if (m_currentNavPoint == 0) {
					m_status = Status::STILL;

					// broadcast the end of the move
					m_entity->sendInternalMessage(gaMessage::END_MOVE);
				}
				else {
					// if we reached the next navpoint, move to the next one
					m_currentNavPoint--;
					p = m_navpoints[m_currentNavPoint];
					direction = p - m_entity->position();
					direction.y = 0;	// move forward, physics will take care of problems
					direction = glm::normalize(direction) * m_speed;

					m_status = Status::MOVE_TO_NEXT_WAYPOINT;
				}
				break;
			}

			// and take action
			if (m_status == Status::MOVE_TO_NEXT_WAYPOINT) {
				m_transforms->m_position = m_entity->position() + direction;
				m_transforms->m_flag = gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL;

				m_entity->sendDelayedMessage(
					gaMessage::WANT_TO_MOVE,
					gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL,
					m_transforms);

				// turn the entity in the direction of the move
				m_entity->sendInternalMessage(
					gaMessage::LOOK_AT,
					-direction);
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
				m_previous.pop_back();

				m_currentNavPoint--;
				p = m_navpoints[m_currentNavPoint];
				direction = p - m_entity->position();
				direction.y = 0;	// move forward, physics will take care of problems
				direction = glm::normalize(direction) * m_speed;

				m_transforms->m_position += direction;
				m_transforms->m_flag = gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL;
				m_entity->sendDelayedMessage(
					gaMessage::WANT_TO_MOVE,
					gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL,
					m_transforms);

				// turn the entity in the direction of the move
				m_entity->sendInternalMessage(
					gaMessage::LOOK_AT,
					-direction);
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
			m_transforms->m_flag = gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL;

			m_entity->sendDelayedMessage(
				gaMessage::WANT_TO_MOVE,
				gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL,
				m_transforms);
		}

		/*
				// and rebuild a path
				if (g_navMesh.findPath(m_previous, m_destination, m_navpoints)) {
					// there is a path
					m_currentNavPoint = m_navpoints.size() - 1;

					m_speed = 0.035f;
					m_transforms->m_position = m_navpoints[m_currentNavPoint];
					m_entity->sendDelayedMessage(
						gaMessage::WANT_TO_MOVE,
						gaMessage::Flag::WANT_TO_MOVE_BREAK_IF_FALL,
						m_transforms);
				}
		*/
		break; }

	case gaMessage::Action::WOULD_FALL:
		// find an other path
		__debugbreak();
		break;
	}
}

/**
 *
 */
void GameEngine::Component::PathFinding::debugGUIinline(void)
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
		ImGui::TreePop();
	}
}