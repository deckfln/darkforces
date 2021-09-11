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
			m_status = Status::MOVE;
			m_destination = glm::vec3(-24.65, 0.07, 30.45);

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
		if (m_status == Status::MOVE) {
			glm::vec3 p = m_navpoints[m_currentNavPoint];

			glm::vec3 direction = p - m_entity->position();
			direction.y = 0;	// move forward, physics will take care of problems

			// did we reach the next navpoint ?
			if (glm::length(direction) < 0.01 && m_currentNavPoint == 0) {
				m_status = Status::STILL;

				// broadcast the end of the move
				m_entity->sendInternalMessage(gaMessage::END_MOVE);
			}
			else {
				if (glm::length(direction) < 0.01) {
					// if we reach the next navpoint, move to the next one
					m_currentNavPoint--;
					p = m_navpoints[m_currentNavPoint];
					direction = p - m_entity->position();
					direction.y = 0;	// move forward, physics will take care of problems
				}

				// and continue to move forward
				float d = glm::length(direction);
				if (d > m_speed) {
					direction = glm::normalize(direction) * m_speed;
				}
				else if (m_currentNavPoint > 0) {
					// move to the next waypoint directly
					m_currentNavPoint--;
					p = m_navpoints[m_currentNavPoint];
					direction = p - m_entity->position();
					direction.y = 0;	// move forward, physics will take care of problems
					direction = glm::normalize(direction) * m_speed;
				}

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
		// lets try to jump to the next way point
		m_currentNavPoint--;
		glm::vec3 p = m_navpoints[m_currentNavPoint];
		glm::vec3 direction = p - m_entity->position();
		direction.y = 0;	// move forward, physics will take care of problems

				// and continue to move forward
		float d = glm::length(direction);
		if (d > m_speed) {
			direction = glm::normalize(direction) * m_speed;
		}

		direction *= m_speed;

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
		break; };

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
		{ Status::MOVE, "Move"}
	};

	if (ImGui::TreeNode(g_className)) {
		ImGui::Text("Status:%s", g_display[m_status]);
		ImGui::Text("Target: %.2f %.2f %.2f", m_destination.x, m_destination.y, m_destination.z);
		ImGui::Text("Speed: %.2f", m_speed);
		ImGui::TreePop();
	}
}