#include "gaPlayer.h"

#include <GLFW/glfw3.h>

#include "../config.h"

#include "../framework/fwCamera.h"

#include "gaActor.h"
#include "gaMessage.h"

#include "../darkforces/dfComponent/dfComponentActor.h"

gaPlayer::gaPlayer(fwCamera *camera, gaActor* actor, float phi):
	fwControlThirdPerson(camera, actor->position(), actor->eyes(), phi, actor->radius()),
	m_entity(actor)
{
	m_actor = (dfComponentActor*)m_entity->findComponent(DF_COMPONENT_ACTOR);
	m_entity->parent(this);
}

/**
 * deal with movement keys
 */
bool gaPlayer::checkKeys(time_t delta)
{
	fwControlThirdPerson::checkKeys(delta);

	if (!m_locked) {
		if (m_currentKeys[GLFW_KEY_X]) {
			// JUMP
			m_velocity.y = 0.004f;
			m_entity->jump(m_velocity);
		}
		if (m_currentKeys[GLFW_KEY_LEFT_CONTROL] && !m_prevKeys[GLFW_KEY_LEFT_CONTROL]) {
			// FIRE
			m_actor->fire(m_lookDirection);
		}
	}

	return true;
}

void gaPlayer::updatePlayer(time_t delta)
{
	if (m_velocity != glm::vec3(0)) {
		m_entity->sendInternalMessage(gaMessage::CONTROLLER, delta, &m_velocity);
	}
	m_position = m_entity->position();
}

gaPlayer::~gaPlayer()
{
}
