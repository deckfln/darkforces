#include "gaController.h"

#include <imgui.h>
#include <GLFW/glfw3.h>

#include "../Debug.h"
#include "../gaMessage.h"
#include "../gaEntity.h"

GameEngine::Component::Controller::Controller(fwCamera* camera,
	const glm::vec3& position,
	float height,
	float phi,
	float radius,
	const std::vector<KeyInfo>& keys):
	gaComponent(gaComponent::Controller),
	fwControlThirdPerson(camera, position, height, phi, radius),
	m_keys(keys)
{
	// add game controler keys
	/*
	m_keys.push_back({ GLFW_KEY_UP, GameEngine::Component::Controller::KeyInfo::Msg::onPress });		// start/stop walking forward walk
	m_keys.push_back({ GLFW_KEY_DOWN, GameEngine::Component::Controller::KeyInfo::Msg::onPress });		// start/stop walking backward walk
	*/
}

GameEngine::Component::Controller::~Controller()
{
}

void GameEngine::Component::Controller::dispatchMessage(gaMessage* message)
{
	switch (message->m_action) {
	case gaMessage::MOVE:
		m_position = m_entity->position();
		break;
	}
}

void GameEngine::Component::Controller::debugGUIinline(void)
{
	if (ImGui::TreeNode("Controller")) {
		ImGui::TreePop();
	}
}

/**
 * deal with movement keys
 */
bool GameEngine::Component::Controller::checkKeys(time_t delta)
{
	fwControlThirdPerson::checkKeys(delta);

	// in recording mode, save the status
#ifdef DEBUG0
	//TODO find how to pass the information to the debugger
	if (m_currentKeys[GLFW_KEY_F1]) {
		g_Debugger.debugMode(true);
	}
#endif

	// inform the entity of the keys it asked for
	for (auto& key : m_keys) {
		switch (key.m_msh) {
		case KeyInfo::Msg::onPressDown:
			if (m_currentKeys[key.m_key] && !m_prevKeys[key.m_key]) {
					m_entity->sendMessage(gaMessage::Action::KEY, key.m_key, m_velocity);
				}
			break;
		case KeyInfo::Msg::onPress:
			if (m_currentKeys[key.m_key]) {
				if (!m_prevKeys[key.m_key]) {
					m_entity->sendMessage(gaMessage::Action::KEY_DOWN, key.m_key, &m_velocity);
				}
				else {
					m_entity->sendMessage(gaMessage::Action::KEY, key.m_key, m_velocity);
				}
			}
			if (!m_currentKeys[key.m_key] && m_prevKeys[key.m_key]) {
				m_entity->sendMessage(gaMessage::Action::KEY_UP, key.m_key, &m_velocity);
			}
			break;
		case KeyInfo::Msg::onPressUp:
			if (!m_currentKeys[key.m_key] && m_prevKeys[key.m_key]) {
				m_entity->sendMessage(gaMessage::Action::KEY, key.m_key, m_velocity);
			}
			break;
		}
	}

	// deal with walk_start/walk_stop
	if (m_currentKeys[GLFW_KEY_UP] && !m_prevKeys[GLFW_KEY_UP]) {
		m_entity->sendMessage(gaMessage::Action::WALK_START);
	}
	if (!m_currentKeys[GLFW_KEY_UP] && m_prevKeys[GLFW_KEY_UP]) {
		m_entity->sendMessage(gaMessage::Action::WALK_STOP);
	}
	if (m_currentKeys[GLFW_KEY_DOWN] && !m_prevKeys[GLFW_KEY_DOWN]) {
		m_entity->sendMessage(gaMessage::Action::WALK_START);
	}
	if (!m_currentKeys[GLFW_KEY_DOWN] && m_prevKeys[GLFW_KEY_DOWN]) {
		m_entity->sendMessage(gaMessage::Action::WALK_STOP);
	}

	return true;
}

void GameEngine::Component::Controller::updatePlayer(time_t delta)
{
	if (m_velocity != glm::vec3(0)) {
		m_entity->sendMessage(gaMessage::CONTROLLER, (uint32_t)delta, m_velocity);
	}

	if (m_eye != m_oldEye || m_lookAt != m_oldLookAt) {
		m_entity->sendMessage(gaMessage::Action::LOOK_AT, m_lookDirection);
		m_entity->sendMessage(gaMessage::Action::MOVE_AT, m_eye);

		m_oldEye = m_eye;
		m_oldLookAt = m_lookAt;
	}
}