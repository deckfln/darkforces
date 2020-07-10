#include "gaPlayer.h"

#include <GLFW/glfw3.h>

#include "../framework/fwCamera.h"
#include "gaActor.h"

gaPlayer::gaPlayer(fwCamera *camera, gaActor* actor, float phi):
	fwControlThirdPerson(camera, actor->position(), actor->eyes(), phi, actor->radius()),
	m_actor(actor)
{
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
			m_actor->jump(m_velocity);
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
	if (m_actor->moveTo(delta, m_velocity)) {
		// freefalling
		m_locked = true;
	}
	else {
		m_locked = false;
	}
	m_position = m_actor->position();
}

gaPlayer::~gaPlayer()
{
}
