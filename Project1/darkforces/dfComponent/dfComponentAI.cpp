#include "dfComponentAI.h"

#include <glm/gtx/rotate_vector.hpp>

#include "../../config.h"
#include "../../gaEngine/gaWorld.h"
#include "../../gaEngine/gaEntity.h"

dfComponentAI::dfComponentAI():
	gaComponent(DF_COMPONENT_AI)
{
	// find the sector the mousebot is running into
	m_direction = glm::normalize(m_direction);
	m_movement = m_direction * 0.01f;
}

void dfComponentAI::dispatchMessage(gaMessage* message)
{

	switch (message->m_action) {
	case GA_MSG_WORLD_INSERT:
		// kick start the AI
		m_center = m_entity->position();
		m_alpha = (rand() / (float)RAND_MAX - 0.f) / 10.0f; // rotation angle to apply to the direction vector
		m_time = rand() % (5 * 30);			// move 5s maximum using the same rotation angle
		m_entity->sendMessage(m_entity->name(), GA_MSG_TIMER, 0, nullptr);
		break;

	case GA_MSG_COLLIDE:
	case GA_MSG_WOULD_FALL:
		m_direction = -m_direction;
		m_entity->sendDelayedMessage(GA_MSG_TIMER);
		break;

	case GA_MSG_TIMER:
		if (--m_time < 0) {
			// after the default delay, change the rotation angle
			m_alpha = (rand() / (float)RAND_MAX - 0.5f) / 10.0f;
			m_time = rand() % (5 * 30);
		}

		m_direction = glm::rotateY(m_direction, m_alpha);
		m_movement = m_direction * 0.06f;
		m_entity->sendMessageToWorld(
			GA_MSG_WANT_TO_MOVE, 
			GA_MSG_WANT_TO_MOVE_BREAK_IF_FALL,		// detects cliff
			&m_movement);
		break;

	case GA_MSG_MOVE: {
		// move request was accepted

		// align the object to the direction
		//convert the direction vector to a quaternion
		glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 right;
		glm::vec3 up;

		if (m_direction != glm::vec3(0)) {
			m_direction = glm::normalize(m_direction);
			right = glm::normalize(glm::cross(_up, m_direction));
			up = glm::cross(m_direction, right);
		}
		else {
			up = _up;
		}

		glm::quat quaternion = glm::quatLookAt(m_direction, up);
		m_entity->sendInternalMessage(GA_MSG_ROTATE, 1, &quaternion);

		// go for next movement
		m_entity->sendDelayedMessage(GA_MSG_TIMER);
		break;
	}

	case DF_MESSAGE_DIES:
		m_active = false;
		break;
	}
}

dfComponentAI::~dfComponentAI()
{
}
