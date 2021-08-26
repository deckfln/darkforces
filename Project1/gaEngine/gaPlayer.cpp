#include "gaPlayer.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#include "../config.h"

#include "../framework/fwCamera.h"

#include "gaActor.h"
#include "gaMessage.h"

#include "../darkforces/dfComponent/dfComponentActor.h"
#include "gaComponent/gaActiveProbe.h"

#ifdef _DEBUG
#include "Debug.h"
#endif

gaPlayer::gaPlayer(fwCamera *camera, gaActor* actor, float phi):
	fwControlThirdPerson(camera, actor->position(), actor->eyes(), phi, actor->radius()),
	m_entity(actor)
{
	m_actor = (dfComponentActor*)m_entity->findComponent(DF_COMPONENT_ACTOR);

	GameEngine::Component::ActiveProbe* probe = new GameEngine::Component::ActiveProbe();

	m_entity->addComponent(probe, gaEntity::Flag::DELETE_AT_EXIT);
	m_entity->parent(this);
}

/**
 * deal with movement keys
 */
bool gaPlayer::checkKeys(time_t delta)
{
	fwControlThirdPerson::checkKeys(delta);

	// in recording mode, save the status
#ifdef _DEBUG
	//TODO find how to pass the information to the debugger
	/*
	if (m_currentKeys[GLFW_KEY_F1]) {
		g_Debugger.debugMode(true);
	}
	*/
#endif

	if (!m_locked) {
		if (m_currentKeys[GLFW_KEY_X]) {
			// JUMP
			m_velocity.y = 0.004f;
			m_entity->jump(m_velocity);
		}
	}

	if (m_currentKeys[GLFW_KEY_LEFT_CONTROL] && !m_prevKeys[GLFW_KEY_LEFT_CONTROL]) {
		// FIRE
		m_actor->fire(m_lookDirection);
	}
	if (m_currentKeys[GLFW_KEY_SPACE]) {
		// refresh components
		m_entity->sendInternalMessage(gaMessage::Action::KEY, 32);
	}

	return true;
}

void gaPlayer::updatePlayer(time_t delta)
{
	if (m_velocity != glm::vec3(0)) {
		m_entity->sendInternalMessage(gaMessage::CONTROLLER, (uint32_t)delta, &m_velocity);
	}
	m_position = m_entity->position();

	if (m_eye != m_oldEye || m_lookAt != m_oldLookAt) {
		m_entity->sendInternalMessage(gaMessage::Action::LOOK_AT, 0, &m_lookAt);
		m_entity->sendInternalMessage(gaMessage::Action::MOVE_AT, 0, &m_eye);

		m_oldEye = m_eye;
		m_oldLookAt = m_lookAt;
	}
}

gaPlayer::~gaPlayer()
{
}
