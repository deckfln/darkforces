#include "gaPlayer.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

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

#ifdef DEBUG
	if (m_replay) {
		std::ifstream myfile;
		myfile.open("player.txt", std::ios::in | std::ios::binary);
		myfile.read((char*)&m_recorder_len, sizeof(m_recorder_len));
		for (int i = 0; i < m_recorder_len; i++)
		{
			myfile.read((char*)&m_recorder[i], sizeof(m_recorder[0]));
		}
		myfile.close();
		m_recorder_end = 0;
	}
#endif
}

/**
 * deal with movement keys
 */
bool gaPlayer::checkKeys(time_t delta)
{
	fwControlThirdPerson::checkKeys(delta);

	// in recording mode, save the status
#ifdef DEBUG
	if (m_currentKeys[GLFW_KEY_F1]) {
		std::ofstream myfile;
		myfile.open("player.txt", std::ios::out | std::ios::binary);
		myfile.write((char*)&m_recorder_len, sizeof(m_recorder_len));
		for (int i = 0, p=m_recorder_start; i< m_recorder_len; i++)
		{
			myfile.write((char *)&m_recorder[p], sizeof(m_recorder[0]));
			p++;
			if (p >= m_recorder.size()) {
				p = 0;
			}
		}
		myfile.close();
	}
	if (m_currentKeys[GLFW_KEY_F2]) {
		m_entity->sendMessageToWorld(gaMessage::SAVE_WORLD, 0, nullptr);
	}
#endif

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

#ifdef DEBUG
	// in record mode
	if (m_record) {
		m_recorder[m_recorder_end++] = m_velocity;
		m_recorder_len++;

		if (m_recorder_len == m_recorder.size()) {
			if (m_recorder_end >= m_recorder.size()) {
				m_recorder_end = 0;
			}

			m_recorder_start++;
			if (m_recorder_start >= m_recorder.size()) {
				m_recorder_start = 0;
			}
		}
	}

	// in replay mode
	if (m_replay) {
		if (m_recorder_end > 275) {
			printf("gaPlayer::updatePlayer\n");
		}
		m_entity->sendInternalMessage(gaMessage::CONTROLLER, delta, &m_recorder[m_recorder_end++]);
		if (m_recorder_end >= m_recorder_len) {
			m_replay = false;
		}
		m_position = m_entity->position();
	}
#endif
}

gaPlayer::~gaPlayer()
{
}
