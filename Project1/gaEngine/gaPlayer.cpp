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
			m_actor->jump();
		}
	}

	return true;
}

void gaPlayer::updatePlayer(time_t delta)
{
	m_actor->rotate(m_velocity);
	if (m_actor->moveTo(delta, m_shift ? 2.0f : 1.0f)) {
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
