#include "PlayerControl.h"

#include <GLFW/glfw3.h>

#include "../gaEngine/World.h"
#include "../myDarkForces.h"

bool DarkForces::PlayerControl::checkKeys(time_t delta)
{
	// Space key can only be sent ONCE
	if (m_currentKeys[GLFW_KEY_SPACE]) {
		if (!m_keySpace) {
			m_parent->keypress(GLFW_KEY_SPACE);
			m_keySpace = true;
		}
	}
	else {
		m_keySpace = false;
	}

	// DEBUG : suspend the timer
	if (m_currentKeys[GLFW_KEY_S]) {
		g_gaWorld.suspendTimer();
	}

	// get the status of the headlight
	if (m_currentKeys[GLFW_KEY_F5]) {
		if (!m_f5) {
			m_f5 = true;
		}
	}
	else {
		m_f5 = false;
	}

	updateCamera(33);			// and move the player if the level changed

	return gaPlayer::checkKeys(delta);
}

DarkForces::PlayerControl::PlayerControl(fwCamera* camera, gaActor* actor, float phi):
	gaPlayer(camera, actor, phi)
{
}

DarkForces::PlayerControl::~PlayerControl()
{
}
