#include "dfComponentAI.h"

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
		g_gaWorld.sendMessageDelayed(m_entity->name(), m_entity->name(), GA_MSG_TIMER, 0, nullptr);
		break;

	case GA_MSG_COLLIDE:
		alpha = 2.0f * 3.1514516f - alpha;
		m_progress = -m_progress;
		m_entity->sendDelayedMessage(GA_MSG_TIMER);
		break;

	case GA_MSG_TIMER:
		alpha += m_progress;

		m_direction = glm::vec3(sin(alpha), 0, cos(alpha));
		m_movement = m_direction * 0.06f ;

		m_entity->sendMessageToWorld(GA_MSG_WANT_TO_MOVE, 0, &m_movement);
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
	}
}

dfComponentAI::~dfComponentAI()
{
}
